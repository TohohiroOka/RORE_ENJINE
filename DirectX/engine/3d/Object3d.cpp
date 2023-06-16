#include "Object3d.h"
#include "Camera.h"
#include "LightGroup.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace DirectX;

std::vector<GraphicsPipelineManager::DrawSet> Object3d::pipeline;

std::unique_ptr<Object3d> Object3d::Create(Model* _model)
{
	// 3Dオブジェクトのインスタンスを生成
	Object3d* instance = new Object3d();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();

	if (_model) {
		instance->SetModel(_model);
	}

	return std::unique_ptr<Object3d>(instance);
}

void Object3d::Initialize()
{
	// nullptrチェック
	assert(device);

	name = typeid(*this).name();

	HRESULT result = S_FALSE;

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(CONST_BUFFER_DATA_B0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));
	if (FAILED(result)) { assert(0); }
}

Object3d::Object3d()
{
	outlineColor = { 1.0f,1.0f, 1.0f };
}

Object3d::~Object3d()
{
	DeleteCollider();
}

void Object3d::Update()
{
	Base3D::Update();
	UpdateWorldMatrix();

	//定数バッファにデータを転送
	CONST_BUFFER_DATA_B0* constMap = nullptr;
	HRESULT result = constBuffB0->Map(0, nullptr, (void**)&constMap);//マッピング
	if (SUCCEEDED(result)) {
		constMap->baseColor = baseColor;
		if (camera)
		{
			constMap->viewproj = camera->GetView() * camera->GetProjection();
			constMap->cameraPos = camera->GetEye();;
		} else
		{
			constMap->viewproj = XMMatrixIdentity();
			constMap->cameraPos = { 0,0,0 };
		}
		constMap->world = matWorld;
		constMap->isBloom = isBloom;
		constMap->isToon = isToon;
		constMap->isOutline = isOutline;
		constMap->isLight = isLight;
		constMap->outlineColor = outlineColor;
		constBuffB0->Unmap(0, nullptr);
	}
}

void Object3d::Draw(const DrawMode _drawMode)
{
	// nullptrチェック
	assert(device);
	assert(Object3d::cmdList);

	// モデルの割り当てがなければ描画しない
	if (model == nullptr) {
		return;
	}

	Update();

	int modeNum = int(_drawMode);

	Base3D::Draw(pipeline[modeNum]);

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// ライトの描画
	light->Draw(cmdList, 2);

	// モデル描画
	model->Draw(cmdList);
}