#include "FbxmManager.h"
#include "Camera.h"
#include "BaseCollider.h"
#include "SafeDelete.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

ID3D12Device* FbxmManager::device = nullptr;
Camera* FbxmManager::camera = nullptr;
LightGroup* FbxmManager::lightGroup = nullptr;
ID3D12GraphicsCommandList* FbxmManager::cmdList = nullptr;
std::unique_ptr<GraphicsPipelineManager> FbxmManager::pipeline = nullptr;

FbxmManager::~FbxmManager()
{
	constBuffB0.Reset();
}

void FbxmManager::CreateGraphicsPipeline()
{
	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xyz���W
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // �@���x�N�g��
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv���W
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//�{�[���ԍ�
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//�{�[���e���x
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	pipeline = GraphicsPipelineManager::Create("Fbx",
		GraphicsPipelineManager::OBJECT_KINDS::FBX, inputLayout, _countof(inputLayout));
}

void FbxmManager::StaticInitialize(ID3D12Device* device)
{
	HRESULT result = S_FALSE;

	// �ď������`�F�b�N
	assert(!FbxmManager::device);

	// nullptr�`�F�b�N
	assert(device);

	FbxmManager::device = device;

	CreateGraphicsPipeline();

	FbxModel::StaticInitialize(device);
}

void FbxmManager::Initialize()
{
	HRESULT result = S_FALSE;
	// �萔�o�b�t�@1�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));
	assert(SUCCEEDED(result));
}

std::unique_ptr<FbxmManager> FbxmManager::Create(FbxModel* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	FbxmManager* instance = new FbxmManager();

	//������
	instance->Initialize();

	//���f�����w�肳��Ă���΃Z�b�g����
	if (model) {
		instance->SetModel(model);
	}

	return std::unique_ptr<FbxmManager>(instance);
}

void FbxmManager::Update()
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
	matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

	const XMMATRIX& matViewProjection = camera->GetView() * camera->GetProjection();
	const XMFLOAT3& cameraPos = camera->GetEye();

	// �萔�o�b�t�@1�փf�[�^�]��
	ConstBufferDataB0* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constMap->isBloom = isBloom;
	constMap->isToon = isToon;
	constMap->isOutline = isOutline;
	constBuffB0->Unmap(0, nullptr);

	model->Update();
}

void FbxmManager::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	FbxmManager::cmdList = cmdList;

	//�p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipeline->pipelineState.Get());

	//���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(pipeline->rootSignature.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	FbxModel::PreDraw(cmdList);
}

void FbxmManager::PostDraw()
{
	// �R�}���h���X�g������
	FbxmManager::cmdList = nullptr;
}

void FbxmManager::Draw()
{
	// nullptr�`�F�b�N
	assert(device);
	assert(FbxmManager::cmdList);

	// ���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
	if (model == nullptr) {
		return;
	}

	//�萔�o�b�t�@���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// ���C�g�̕`��
	lightGroup->Draw(cmdList, 3);

	// ���f���`��
	model->Draw(cmdList);
}

void FbxmManager::Finalize()
{
	FbxModel::Finalize();
	pipeline.reset();
}