#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

#include "GraphicsPipelineManager.h"
#include "Model.h"
#include "Texture.h"
#include "CollisionInfo.h"

class BaseCollider;
class Camera;
class LightGroup;

class InterfaceObject3d
{
protected:// �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

protected:// �T�u�N���X

	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0
	{
		XMMATRIX viewproj; // �r���[�v���W�F�N�V�����s��
		XMMATRIX world; // ���[���h�s��
		XMFLOAT3 cameraPos; // �J�������W�i���[���h���W�j
		unsigned int isSkinning;//�X�L�j���O���s����
		unsigned int isBloom;//�u���[���̗L��
		unsigned int isToon;//�g�D�[���̗L��
		unsigned int isOutline;//�A�E�g���C���̗L��
	};

	// �萔�o�b�t�@�p�f�[�^�\����B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 baseColor = { 1,1,1 };//�x�[�X�J���\
		float pad1; // �p�f�B���O
		XMFLOAT3 ambient; // �A���r�G���g�W��
		float pad2; // �p�f�B���O
		XMFLOAT3 diffuse; // �f�B�t���[�Y�W��
		float metalness; // �����x(0 or 1)
		float specular; // ���ʔ��˓x
		float roughness; // �e��
		float alpha;	// �A���t�@
	};

public:

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// �R�}���h���X�g�̃Z�b�g
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	static void SetCmdList(ID3D12GraphicsCommandList* cmdList) { InterfaceObject3d::cmdList = cmdList; }

	/// <summary>
	/// �R�}���h���X�g�̃Z�b�g
	/// </summary>
	static void ReleaseCmdList() { InterfaceObject3d::cmdList = nullptr; }

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="camera">�J����</param>
	static void SetCamera(Camera* camera) { InterfaceObject3d::camera = camera; }

	/// <summary>
	/// ���C�g�O���[�v�̃Z�b�g
	/// </summary>
	/// <param name="lightGroup">���C�g�O���[�v</param>
	static void SetLightGroup(LightGroup* light) { InterfaceObject3d::light = light; }

	/// <summary>
	/// �L���[�u�}�b�v�̃Z�b�g
	/// </summary>
	/// <param name="cubetex">�L���[�u�}�b�v�e�N�X�`��</param>
	static void SetLightGroup(Texture* cubetex) { InterfaceObject3d::cubetex = cubetex; }

public:

	InterfaceObject3d() = default;
	virtual ~InterfaceObject3d();

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// �����蔻��̕`��
	/// </summary>
	virtual void ColliderDraw();

	/// <summary>
	/// �s��̍X�V
	/// </summary>
	void UpdateWorldMatrix();

	/// <summary>
	/// �Փˎ��R�[���o�b�N�֐�
	/// </summary>
	/// <param name="info">�Փˏ��</param>
	virtual void OnCollision(const CollisionInfo& info) {}

protected:

	// �f�o�C�X
	static ID3D12Device* device;
	//�R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	//�J����
	static Camera* camera;
	//���C�g
	static LightGroup* light;
	//�A�E�g���C���̐F
	static XMFLOAT4 outlineColor;
	//�A�E�g���C���̕�
	static float outlineWidth;
	//�L���[�u�}�b�v
	static Texture* cubetex;

protected:

	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB0;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB1;
	//�萔�o�b�t�@�ɓn���ϐ�
	ConstBufferDataB1 constBufferB1Num;
	//�u���[���̗L��
	bool isBloom = false;
	//�g�D�[���̗L��
	bool isToon = false;
	//�A�E�g���C���̗L��
	bool isOutline = false;
	//�X�L�j���O
	bool isSkinning = false;
	// ���f��
	Model* model = nullptr;
	// �R���C�_�[
	BaseCollider* collider = nullptr;
	// �e�I�u�W�F�N�g
	InterfaceObject3d* parent = nullptr;
	// ���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld = {};
	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z�����̃��[�J����]�p
	XMFLOAT3 rotation = { 0,0,0 };
	// ���[�J�����W
	XMFLOAT3 position = { 0,0,0 };

public:

	/// <summary>
	/// ���W�̎擾
	/// </summary>
	/// <returns>���W</returns>
	const XMFLOAT3& GetPosition() { return position; }

	/// <summary>
	/// ��]�̎擾
	/// </summary>
	/// <returns>��]</returns>
	const XMFLOAT3& GetRotation() { return rotation; }

	/// <summary>
	/// �傫���̎擾
	/// </summary>
	/// <returns>�傫��</returns>
	const XMFLOAT3& GetScale() { return scale; }

	/// <summary>
	/// ���W�̐ݒ�
	/// </summary>
	/// <param name="position">���W</param>
	void SetPosition(XMFLOAT3 position) { this->position = position; }

	/// <summary>
	/// ��]�p�̐ݒ�
	/// </summary>
	/// <param name="rotation">��]�p</param>
	void SetRotation(XMFLOAT3 rotation) { this->rotation = rotation; }

	/// <summary>
	/// �X�P�[���̐ݒ�
	/// </summary>
	/// <param name="position">�X�P�[��</param>
	void SetScale(XMFLOAT3 scale) { this->scale = scale; }

	/// <summary>
	/// �u���[���̃Z�b�g
	/// </summary>
	/// <param name="isBloom">�u���[���L->true / ��->false</param>
	void SetBloom(bool isBloom) { this->isBloom = isBloom; }

	/// <summary>
	/// �g�D�[���̃Z�b�g
	/// </summary>
	/// <param name="isToon">�g�D�[���L->true / ��->false</param>
	void SetToon(bool isToon) { this->isToon = isToon; }

	/// <summary>
	/// �A�E�g���C���̃Z�b�g
	/// </summary>
	/// <param name="isOutline">�A�E�g���C���L->true / ��->false</param>
	void SetOutline(bool isOutline) { this->isOutline = isOutline; }

	/// <summary>
	/// �A�E�g���C���̐F�Z�b�g
	/// </summary>
	/// <param name="outlineColor">��</param>
	static void SetOutlineColor(XMFLOAT4 outlineColor) { InterfaceObject3d::outlineColor = outlineColor; }

	/// <summary>
	/// �A�E�g���C���̕��Z�b�g
	/// </summary>
	/// <param name="outlineWidth">��</param>
	static void SetOutlineWidth(float outlineWidth) { InterfaceObject3d::outlineWidth = outlineWidth; }

	/// <summary>
	/// �A�E�g���C���̐F�擾
	/// </summary>
	/// <param name="outlineColor">��</param>
	static XMFLOAT4 GetOutlineColor() { return outlineColor; }

	/// <summary>
	/// �A�E�g���C���̕��擾
	/// </summary>
	/// <param name="outlineWidth">��</param>
	static float GetOutlineWidth() { return outlineWidth; }

	/// <summary>
	/// �R���C�_�[�̃Z�b�g
	/// </summary>
	/// <param name="collider">�R���C�_�[</param>
	void SetCollider(BaseCollider* collider);

	/// <summary>
	/// �R���C�_�[�̎擾
	/// </summary>
	/// <returns>�R���C�_�[</returns>
	BaseCollider* GetCollider() { return collider; }

	/// <summary>
	/// ���[���h�s��̎擾
	/// </summary>
	/// <returns>���[���h�s��</returns>
	const XMMATRIX& GetMatWorld() { return matWorld; }

	/// <summary>
	/// ���_�̃Z�b�g
	/// </summary>
	/// <param name="model">���_</param>
	void SetMatWorld(XMMATRIX matWorld) { this->matWorld = matWorld; }

	/// <summary>
	/// ���[���h���W���擾
	/// </summary>
	/// <returns></returns>
	XMFLOAT3 GetWorldPosition();

};