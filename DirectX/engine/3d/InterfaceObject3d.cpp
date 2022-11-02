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

void InterfaceObject3d::StaticInitialize(ID3D12Device* _device)
{
	// �������`�F�b�N
	assert(!InterfaceObject3d::device);

	// nullptr�`�F�b�N
	assert(_device);

	InterfaceObject3d::device = _device;

	// ���f���̐ÓI������
	Model::StaticInitialize(_device);
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
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(CONST_BUFFER_DATA_B0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));
	if (FAILED(result)) { assert(0); }
}

void InterfaceObject3d::Update()
{
	UpdateWorldMatrix();

	//�萔�o�b�t�@�Ƀf�[�^��]��
	CONST_BUFFER_DATA_B0* constMap = nullptr;
	HRESULT result = constBuffB0->Map(0, nullptr, (void**)&constMap);//�}�b�s���O
	if (SUCCEEDED(result)) {
		constMap->baseColor = baseColor;
		if (camera)
		{
			constMap->viewproj = camera->GetView() * camera->GetProjection();
			constMap->cameraPos = camera->GetEye();;
		}
		else
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

	// �����蔻��X�V
	if (collider) {
		collider->Update();
	}
}

void InterfaceObject3d::Draw()
{
	Update();

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// ���C�g�̕`��
	light->Draw(cmdList, 2);
}

void InterfaceObject3d::ColliderDraw()
{
	collider->Draw();
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
		XMFLOAT3 pPos = parent->GetPosition();
		XMFLOAT3 pRota = parent->GetRotation();
		// �e�I�u�W�F�N�g�̃��[���h�s����|����
		matWorld *= XMMatrixTranslation(-pPos.x, -pPos.y, -pPos.z);
		matWorld *= XMMatrixRotationZ(XMConvertToRadians(pRota.z));
		matWorld *= XMMatrixRotationZ(XMConvertToRadians(pRota.x));
		matWorld *= XMMatrixRotationZ(XMConvertToRadians(pRota.y));
		matWorld *= XMMatrixTranslation(pPos.x, pPos.y, pPos.z);

		//XMFLOAT3 radiun = { asinf(-matWorld.r[2].m128_f32[1]),
		//	asinf(-matWorld.r[0].m128_f32[2]),asinf(-matWorld.r[1].m128_f32[0]) };
		//rotation = { XMConvertToDegrees(radiun.x),XMConvertToDegrees(radiun.y) ,XMConvertToDegrees(radiun.z) };
		//position = { matWorld.r[3].m128_f32[0],matWorld.r[3].m128_f32[1],matWorld.r[3].m128_f32[2] };

		//rotation = { rotation.x + pRota.x,rotation.y + pRota.y, rotation.z + pRota.z };

		//matWorld *= parent->GetMatWorld();
	}
}

void InterfaceObject3d::SetCollider(BaseCollider* _collider)
{
	_collider->SetObject(this);
	this->collider = _collider;
	// �R���W�����}�l�[�W���ɒǉ�
	CollisionManager::GetInstance()->AddCollider(_collider);

	UpdateWorldMatrix();
	_collider->Update();
}

XMFLOAT3 InterfaceObject3d::GetWorldPosition()
{
	XMFLOAT3 worldpos;

	worldpos.x = matWorld.r[3].m128_f32[0];
	worldpos.y = matWorld.r[3].m128_f32[1];
	worldpos.z = matWorld.r[3].m128_f32[2];

	return worldpos;
}