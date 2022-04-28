#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

#include "PmxModel.h"
#include "LightGroup.h"
#include "GraphicsPipelineManager.h"

class Camera;

class Pmx
{
protected: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X

	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0
	{
		XMFLOAT4 color;
		XMMATRIX viewproj; // �r���[�v���W�F�N�V�����s��
		XMMATRIX world; // ���[���h�s��
		XMFLOAT3 cameraPos; // �J�������W�i���[���h���W�j
		unsigned int isBloom;//�u���[���̗L��
		unsigned int isToon;//�g�D�[���̗L��
		unsigned int isOutline;//�A�E�g���C���̗L��
	};

private: // �ÓI�����o�֐�

	/// <summary>
	/// �O���t�B�b�N�p�C�v���C���̐���
	/// </summary>
	static void CreateGraphicsPipeline();

public: // �ÓI�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="camera">�J����</param>
	static void SetCamera(Camera* camera) {
		Pmx::camera = camera;
	}

	/// <summary>
	/// ���C�g�O���[�v�̃Z�b�g
	/// </summary>
	/// <param name="lightGroup">���C�g�O���[�v</param>
	static void SetLightGroup(LightGroup* lightGroup) {
		Pmx::lightGroup = lightGroup;
	}

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 3D�I�u�W�F�N�g����
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<Pmx> Create(PmxModel* model);

	/// <summary>
	/// �������
	/// </summary>
	static void Finalize();

private: // �ÓI�����o�ϐ�

	// �f�o�C�X
	static ID3D12Device* device;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	//�p�C�v���C��
	static std::unique_ptr<GraphicsPipelineManager> pipeline;
	// �J����
	static Camera* camera;
	// ���C�g
	static LightGroup* lightGroup;
	//�A�E�g���C���̐F
	static XMFLOAT4 outlineColor;
	//�A�E�g���C���̕�
	static float outlineWidth;

public: // �����o�֐�

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Pmx() = default;

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~Pmx();

	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	virtual bool Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// �s��̍X�V
	/// </summary>
	void UpdateWorldMatrix();

protected: // �����o�ϐ�

	// ���O
	const char* name = nullptr;
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB0;
	// �F
	XMFLOAT4 color = { 1,1,1,1 };
	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z�����̃��[�J����]�p
	XMFLOAT3 rotation = { 0,0,0 };
	// ���[�J�����W
	XMFLOAT3 position = { 0,0,0 };
	// ���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld = {};
	//�u���[���̗L��
	bool isBloom = false;
	//�g�D�[���̗L��
	bool isToon = false;
	//�A�E�g���C���̗L��
	bool isOutline = false;
	// �e�I�u�W�F�N�g
	Pmx* parent = nullptr;
	//// ���f��
	PmxModel* model;

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
	/// �F�̎擾
	/// </summary>
	/// <returns>�F</returns>
	const XMFLOAT4& GetColor() { return color; }

	/// <summary>
	/// �傫���̎擾
	/// </summary>
	/// <returns>�傫��</returns>
	const XMFLOAT3& GetScale() { return scale; }

	/// <summary>
	/// ���[���h�s��̎擾
	/// </summary>
	/// <returns>���[���h�s��</returns>
	const XMMATRIX& GetMatWorld() { return matWorld; }

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
	/// �F�̐ݒ�
	/// </summary>
	/// <param name="color">�F</param>
	void SetColor(XMFLOAT4 color) { this->color = color; }

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
	static void SetOutlineColor(XMFLOAT4 outlineColor) { Pmx::outlineColor = outlineColor; }

	/// <summary>
	/// �A�E�g���C���̕��Z�b�g
	/// </summary>
	/// <param name="outlineWidth">��</param>
	static void SetOutlineWidth(float outlineWidth) { Pmx::outlineWidth = outlineWidth; }

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="model">���f��</param>
	void SetModel(PmxModel * model) { this->model = model; }

	/// <summary>
	/// ���[���h���W���擾
	/// </summary>
	/// <returns></returns>
	XMFLOAT3 GetWorldPosition();

	/// <summary>
	/// ���f�����擾
	/// </summary>
	/// <returns>���f��</returns>
	inline PmxModel* GetModel() { return model; }

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
};

