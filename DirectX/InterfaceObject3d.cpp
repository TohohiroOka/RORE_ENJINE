#include "InterfaceObject3d.h"
#include "BaseCollider.h"
#include "CollisionManager.h"
#include "Camera.h"
#include "LightGroup.h"
#include "Model.h"
#include "Texture.h"

ID3D12Device* InterfaceObject3d::device = nullptr;
ID3D12GraphicsCommandList* InterfaceObject3d::cmdList = nullptr;
Camera* InterfaceObject3d::camera = nullptr;
LightGroup* InterfaceObject3d::light = nullptr;
DirectX::XMFLOAT4 InterfaceObject3d::outlineColor;
float InterfaceObject3d::outlineWidth;
Texture* InterfaceObject3d::cubetex;

using namespace DirectX;
using namespace Microsoft::WRL;

void InterfaceObject3d::StaticInitialize(ID3D12Device* device)
{
	// �������`�F�b�N
	assert(!InterfaceObject3d::device);

	// nullptr�`�F�b�N
	assert(device);

	InterfaceObject3d::device = device;

	// ���f���̐ÓI������
	Model::StaticInitialize(device);
}

InterfaceObject3d::~InterfaceObject3d()
{
	//�R���C�_�[���
	if (collider) {
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}

	constBuffB0.Reset();
	constBuffB1.Reset();
}

void InterfaceObject3d::Initialize()
{
	HRESULT result = S_FALSE;

	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1));
	assert(SUCCEEDED(result));
}

void InterfaceObject3d::Update()
{
	assert(camera);

	UpdateWorldMatrix();

	const XMMATRIX& matViewProjection = camera->GetView() * camera->GetProjection();
	const XMFLOAT3& cameraPos = camera->GetEye();

	//�萔�o�b�t�@�Ƀf�[�^��]��
	ConstBufferDataB0* constMap = nullptr;
	HRESULT result = constBuffB0->Map(0, nullptr, (void**)&constMap);//�}�b�s���O
	assert(SUCCEEDED(result));
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constMap->isSkinning = isSkinning;
	constMap->isBloom = isBloom;
	constMap->isToon = isToon;
	constMap->isOutline = isOutline;
	constBuffB0->Unmap(0, nullptr);

	ConstBufferDataB1* constMapB1 = nullptr;
	result = constBuffB1->Map(0, nullptr, (void**)&constMapB1);//�}�b�s���O
	assert(SUCCEEDED(result));
	constMapB1->baseColor = constBufferB1Num.baseColor;
	constMapB1->ambient = constBufferB1Num.ambient;
	constMapB1->diffuse = constBufferB1Num.diffuse;
	constMapB1->metalness = constBufferB1Num.metalness;
	constMapB1->specular = constBufferB1Num.specular;
	constMapB1->roughness = constBufferB1Num.roughness;
	constMapB1->alpha = constBufferB1Num.alpha;;
	constBuffB1->Unmap(0, nullptr);

	// �����蔻��X�V
	if (collider) {
		collider->Update();
	}
}

void InterfaceObject3d::Draw()
{
	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	// ���C�g�̕`��
	light->Draw(cmdList, 2);
}

void InterfaceObject3d::UpdateWorldMatrix()
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

void InterfaceObject3d::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	// �R���W�����}�l�[�W���ɒǉ�
	CollisionManager::GetInstance()->AddCollider(collider);

	UpdateWorldMatrix();
	collider->Update();
}

XMFLOAT3 InterfaceObject3d::GetWorldPosition()
{
	XMFLOAT3 worldpos;

	worldpos.x = matWorld.r[3].m128_f32[0];
	worldpos.y = matWorld.r[3].m128_f32[1];
	worldpos.z = matWorld.r[3].m128_f32[2];

	return worldpos;
}