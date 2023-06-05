#include "Object3d.h"

#include "BaseCollider.h"
#include "CollisionManager.h"
#include "Camera.h"
#include "LightGroup.h"
#include "Model.h"
#include "Texture.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace DirectX;
using namespace Microsoft::WRL;

GraphicsPipelineManager::GRAPHICS_PIPELINE Object3d::pipeline;

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

void Object3d::Update()
{
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
		constMap->isSkinning = isSkinning;
		constMap->isBloom = isBloom;
		constMap->isToon = isToon;
		constMap->isOutline = isOutline;
		constMap->isLight = isLight;
		constBuffB0->Unmap(0, nullptr);
	}
}

void Object3d::PreDraw()
{
	// パイプラインステートの設定
	cmdList->SetPipelineState(pipeline.pipelineState.Get());

	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipeline.rootSignature.Get());

	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object3d::Draw()
{
	// nullptrチェック
	assert(device);
	assert(Object3d::cmdList);

	// モデルの割り当てがなければ描画しない
	if (model == nullptr) {
		return;
	}

	InterfaceObject3d::Draw();

	// モデル描画
	model->Draw(cmdList);
}