#include "HeightMap.h"
#include "Easing.h"
#include "Camera.h"
#include "LightGroup.h"
#include "Texture.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace DirectX;

std::vector<GraphicsPipelineManager::DrawSet> HeightMap::pipeline;

std::unique_ptr<HeightMap> HeightMap::Create(TerrainModel* _model)
{
	//インスタンスを生成
	HeightMap* instance = new HeightMap();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->SetModel(_model);

	//初期化
	instance->Initialize();

	return std::unique_ptr<HeightMap>(instance);
}

void HeightMap::Initialize()
{
	// nullptrチェック
	assert(device);

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

HeightMap::~HeightMap()
{
	DeleteCollider();
}

void HeightMap::Update()
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
		constMap->isSkinning = isSkinning;
		constMap->isBloom = isBloom;
		constMap->isToon = isToon;
		constMap->isOutline = isOutline;
		constMap->isLight = isLight;
		constBuffB0->Unmap(0, nullptr);
	}
}

void HeightMap::Draw(const DrawMode _drawMode)
{
	Update();

	int modeNum= int(_drawMode);

	Base3D::Draw(pipeline[modeNum]);

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// ライトの描画
	light->Draw(cmdList, 2);

	model->Draw(cmdList);
}

void HeightMap::SetChangeDrawModel(TerrainModel* _beforeModel, TerrainModel* afterModel, const float _ratio)
{
	std::vector<Mesh::VERTEX> verticesC = model->GetModel()->GetMeshes()[0]->GetVertices();

	const std::array<std::vector<Mesh::VERTEX>, 2> vertices = {
		_beforeModel->GetModel()->GetMeshes()[0]->GetVertices(),
		afterModel->GetModel()->GetMeshes()[0]->GetVertices() };

	int i = 0;
	Mesh& nowMesh = *model->GetModel()->GetMeshes()[0];

	if(_ratio<1){
		for (auto& vertex : verticesC)
		{
			vertex.pos = Easing::Lerp(vertices[0][i].pos, vertices[1][i].pos, _ratio);
			vertex.normal = Easing::Lerp(vertices[0][i].normal, vertices[1][i].normal, _ratio);

			nowMesh.ChangeVertex(vertex, i);
			i++;
		}
	} else {
		for (auto& vertex : verticesC)
		{
			vertex.pos =vertices[1][i].pos;
			vertex.normal =  vertices[1][i].normal;
			nowMesh.ChangeVertex(vertex, i);
		}
	}
	nowMesh.SendVertex();
}

void HeightMap::SetChangeHitModel(TerrainModel* _beforeModel, TerrainModel* afterModel, const float _ratio)
{
	std::vector<Mesh::VERTEX> verticesC = model->GetHitVertices();

	const std::array<std::vector<Mesh::VERTEX>, 2> vertices = {
		_beforeModel->GetHitVertices(),
		afterModel->GetHitVertices() };

	int i = 0;

	if (_ratio < 1) {
		for (auto& vertex : verticesC)
		{
			vertex.pos = Easing::Lerp(vertices[0][i].pos, vertices[1][i].pos, _ratio);
			vertex.normal = Easing::Lerp(vertices[0][i].normal, vertices[1][i].normal, _ratio);

			model->SetHitVertices(vertex, i);
			i++;
		}
	} else {
		for (auto& vertex : verticesC)
		{
			vertex.pos = vertices[1][i].pos;
			vertex.normal = vertices[1][i].normal;
			model->SetHitVertices(vertex, i);
		}
	}
}