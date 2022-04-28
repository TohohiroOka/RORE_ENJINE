#include "Pmx.h"
#include "Camera.h"

using namespace Microsoft::WRL;
using namespace DirectX;

ID3D12Device* Pmx::device = nullptr;
ID3D12GraphicsCommandList* Pmx::cmdList = nullptr;
Camera* Pmx::camera = nullptr;
LightGroup* Pmx::lightGroup = nullptr;
std::unique_ptr<GraphicsPipelineManager> Pmx::pipeline;

void Pmx::StaticInitialize(ID3D12Device* device)
{
	// �������`�F�b�N
	assert(!Pmx::device);

	// nullptr�`�F�b�N
	assert(device);

	Pmx::device = device;

	PmxModel::StaticInitialize(device);

	// �O���t�B�b�N�p�C�v���C���̐���
	CreateGraphicsPipeline();
}

void Pmx::CreateGraphicsPipeline()
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
	};

	pipeline = GraphicsPipelineManager::Create("Object3d",
		GraphicsPipelineManager::OBJECT_KINDS::OBJ, inputLayout, _countof(inputLayout));
}

bool Pmx::Initialize()
{
	// nullptr�`�F�b�N
	assert(device);

	name = typeid(*this).name();

	HRESULT result;
	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	return true;
}

std::unique_ptr<Pmx> Pmx::Create(PmxModel* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Pmx* instance = new Pmx();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
	}

	if (model) {
		instance->SetModel(model);
	}

	return std::unique_ptr<Pmx>(instance);
}

void Pmx::Update()
{
	assert(Pmx::camera);

	HRESULT result;

	UpdateWorldMatrix();

	const XMMATRIX& matViewProjection = camera->GetView() * camera->GetProjection();
	const XMFLOAT3& cameraPos = camera->GetEye();

	// �萔�o�b�t�@�փf�[�^�]��
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
}

void Pmx::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
	assert(Pmx::cmdList == nullptr);

	// �R�}���h���X�g���Z�b�g
	Pmx::cmdList = cmdList;

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipeline->pipelineState.Get());

	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(pipeline->rootSignature.Get());

	// �v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Pmx::PostDraw()
{
	// �R�}���h���X�g������
	Pmx::cmdList = nullptr;
}

void Pmx::Draw()
{
	// nullptr�`�F�b�N
	assert(device);
	assert(Pmx::cmdList);

	// ���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
	if (model == nullptr) {
		return;
	}

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// ���C�g�̕`��
	lightGroup->Draw(cmdList, 3);

	// ���f���`��
	model->Draw(cmdList);
}

void Pmx::UpdateWorldMatrix()
{
	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
	matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

	// �e�I�u�W�F�N�g�������
	if (parent != nullptr) {
		// �e�I�u�W�F�N�g�̃��[���h�s����|����
		matWorld *= parent->matWorld;
	}
}

Pmx::~Pmx()
{
	constBuffB0.Reset();
}
