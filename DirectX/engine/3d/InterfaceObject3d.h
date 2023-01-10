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
	struct CONST_BUFFER_DATA_B0
	{
		XMFLOAT4 baseColor;//�x�[�X�J���[
		XMMATRIX viewproj; // �r���[�v���W�F�N�V�����s��
		XMMATRIX world; // ���[���h�s��
		XMFLOAT3 cameraPos; // �J�������W�i���[���h���W�j
		unsigned int isSkinning;//�X�L�j���O���s����
		unsigned int isBloom;//�u���[���̗L��
		unsigned int isToon;//�g�D�[���̗L��
		unsigned int isOutline;//�A�E�g���C���̗L��
		unsigned int isLight;//���C�e�B���O�̗L��
	};

public:

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="_device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* _device);

	/// <summary>
	/// �R�}���h���X�g�̃Z�b�g
	/// </summary>
	/// <param name="_cmdList">�R�}���h���X�g</param>
	static void SetCmdList(ID3D12GraphicsCommandList* _cmdList) { InterfaceObject3d::cmdList = _cmdList; }

	/// <summary>
	/// �R�}���h���X�g�̃Z�b�g
	/// </summary>
	static void ReleaseCmdList() { InterfaceObject3d::cmdList = nullptr; }

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="_camera">�J����</param>
	static void SetCamera(Camera* _camera) { InterfaceObject3d::camera = _camera; }

	/// <summary>
	/// ���C�g�O���[�v�̃Z�b�g
	/// </summary>
	/// <param name="_light">���C�g�O���[�v</param>
	static void SetLightGroup(LightGroup* _light) { InterfaceObject3d::light = _light; }

	/// <summary>
	/// �L���[�u�}�b�v�̃Z�b�g
	/// </summary>
	/// <param name="_cubetex">�L���[�u�}�b�v�e�N�X�`��</param>
	static void SetLightGroup(Texture* _cubetex) { InterfaceObject3d::cubetex = _cubetex; }

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

public:

	InterfaceObject3d() = default;
	virtual ~InterfaceObject3d();

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize() = 0;

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
	/// <param name="_info">�Փˏ��</param>
	virtual void OnCollision(const CollisionInfo& _info) {}

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
	//�x�[�X�J���[
	XMFLOAT4 baseColor = { 1,1,1,1 };
	//�u���[���̗L��
	bool isBloom = false;
	//�g�D�[���̗L��
	bool isToon = false;
	//�A�E�g���C���̗L��
	bool isOutline = false;
	//�X�L�j���O
	bool isSkinning = false;
	//���C�e�B���O
	bool isLight = true;
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
	/// <param name="_position">���W</param>
	void SetPosition(const XMFLOAT3& _position) { this->position = _position; }

	/// <summary>
	/// ��]�p�̐ݒ�
	/// </summary>
	/// <param name="_rotation">��]�p</param>
	void SetRotation(const XMFLOAT3& _rotation) { this->rotation = _rotation; }

	/// <summary>
	/// �X�P�[���̐ݒ�
	/// </summary>
	/// <param name="_scale">�X�P�[��</param>
	void SetScale(const XMFLOAT3& _scale) { this->scale = _scale; }

	/// <summary>
	/// �X�P�[���̐ݒ�
	/// </summary>
	/// <param name="_color">�F</param>
	void SetColor(const XMFLOAT4& _color) { this->baseColor = _color; }

	/// <summary>
	/// �u���[���̃Z�b�g
	/// </summary>
	/// <param name="_isBloom">�u���[���L->true / ��->false</param>
	void SetBloom(bool _isBloom) { this->isBloom = _isBloom; }

	/// <summary>
	/// �g�D�[���̃Z�b�g
	/// </summary>
	/// <param name="_isToon">�g�D�[���L->true / ��->false</param>
	void SetToon(bool _isToon) { this->isToon = _isToon; }

	/// <summary>
	/// ���C�e�B���O�̃Z�b�g
	/// </summary>
	/// <param name="_isLight">���C�e�B���O�L->true / ��->false</param>
	void SetLight(bool _isLight) { this->isLight = _isLight; }

	/// <summary>
	/// �A�E�g���C���̃Z�b�g
	/// </summary>
	/// <param name="_isOutline">�A�E�g���C���L->true / ��->false</param>
	void SetOutline(bool _isOutline) { this->isOutline = _isOutline; }

	/// <summary>
	/// �A�E�g���C���̐F�Z�b�g
	/// </summary>
	/// <param name="_outlineColor">��</param>
	static void SetOutlineColor(const XMFLOAT4& _outlineColor) { InterfaceObject3d::outlineColor = _outlineColor; }

	/// <summary>
	/// �A�E�g���C���̕��Z�b�g
	/// </summary>
	/// <param name="_outlineWidth">��</param>
	static void SetOutlineWidth(float _outlineWidth) { InterfaceObject3d::outlineWidth = _outlineWidth; }

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
	/// <param name="_collider">�R���C�_�[</param>
	void SetCollider(BaseCollider* _collider);

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
	/// ���[���h�s��̃Z�b�g
	/// </summary>
	/// <param name="_matWorld">���[���h�s��</param>
	void SetMatWorld(const XMMATRIX& _matWorld) { this->matWorld = _matWorld; }

	/// <summary>
	/// ���[���h���W���擾
	/// </summary>
	/// <returns></returns>
	XMFLOAT3 GetWorldPosition();

	/// <summary>
	/// �e�I�u�W�F�N�g�̃Z�b�g
	/// </summary>
	void SetParent(InterfaceObject3d* _parent) { parent = _parent; }
};