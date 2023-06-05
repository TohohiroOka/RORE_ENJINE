#include "HeightMap.h"
#include "Camera.h"
#include "SafeDelete.h"
#include "Easing.h"

using namespace Microsoft::WRL;
using namespace DirectX;

GraphicsPipelineManager::GRAPHICS_PIPELINE HeightMap::pipeline;

std::unique_ptr<HeightMap> HeightMap::Create(TerrainModel* _model)
{
	//�C���X�^���X�𐶐�
	HeightMap* instance = new HeightMap();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->SetModel(_model);

	//������
	instance->Initialize();

	return std::unique_ptr<HeightMap>(instance);
}

void HeightMap::PreDraw()
{
	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipeline.pipelineState.Get());

	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(pipeline.rootSignature.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void HeightMap::Initialize()
{
	// nullptr�`�F�b�N
	assert(device);

	name = typeid(*this).name();

	HRESULT result = S_FALSE;

	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(CONST_BUFFER_DATA_B0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));
	if (FAILED(result)) { assert(0); }
}

void HeightMap::Update()
{
	//�萔�o�b�t�@�Ƀf�[�^��]��
	CONST_BUFFER_DATA_B0* constMap = nullptr;
	HRESULT result = constBuffB0->Map(0, nullptr, (void**)&constMap);//�}�b�s���O
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

void HeightMap::Draw()
{
	InterfaceObject3d::Draw();

	cmdList->SetGraphicsRootConstantBufferView(3, constBufferOData->GetGPUVirtualAddress());

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
