#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include "GraphicsPipelineManager.h"
#include "Texture.h"
#include "Model.h"

class Camera;
class LightGroup;

class InstanceObject
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	//���_�f�[�^3D
	struct VERTEX {
		XMFLOAT3 pos;
		XMFLOAT3 uv;
		XMFLOAT3 normal;
	};

	//��x�ɕ`��ł����
	static const int draw_max_num = 512;

	struct OBJECT_INFO
	{
		XMFLOAT4 baseColor[draw_max_num];//�x�[�X�J���[
		XMMATRIX matWorld[draw_max_num];//world�s��
	};

	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct CONST_BUFFER_DATA_B0
	{
		XMMATRIX viewproj; // �r���[�v���W�F�N�V�����s��
		XMFLOAT3 cameraPos; // �J�������W�i���[���h���W�j
		unsigned int isBloom;//�u���[���̗L��
		unsigned int isToon;//�g�D�[���̗L��
		unsigned int isOutline;//�A�E�g���C���̗L��
		unsigned int isLight;//���C�e�B���O�̗L��
	};

public://�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="_device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* _device);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="_model">���f��</param>
	/// <returns>�C���X�^���X</returns>
	static std::unique_ptr<InstanceObject> Create(Model* _model);

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="_camera">�J����</param>
	static void SetCamera(Camera* _camera) { InstanceObject::camera = _camera; }

	/// <summary>
	/// ���C�g�O���[�v�̃Z�b�g
	/// </summary>
	/// <param name="_light">���C�g�O���[�v</param>
	static void SetLightGroup(LightGroup* _light) { InstanceObject::light = _light; }

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="_cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* _cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

private://�����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_model">���f��</param>
	void Initialize(Model* _model);

public:

	InstanceObject() {};
	~InstanceObject();

	/// <summary>
	/// �`��Z�b�g
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_scale">�傫��</param>
	/// <param name="_rotation">��]�p</param>
	/// <param name="_color">�F</param>
	void DrawInstance(const XMFLOAT3& _pos, const XMFLOAT3& _scale,
		const XMFLOAT3& _rotation, const XMFLOAT4& _color);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// �p�C�v���C���̃Z�b�g
	/// </summary>
	/// <param name="_pipeline">�p�C�v���C��</param>
	static void SetPipeline(const GraphicsPipelineManager::GRAPHICS_PIPELINE& _pipeline) { pipeline = _pipeline; }

	/// <summary>
	/// �C���X�^���V���O�`��ő�`�搔�ɂȂ��Ă��Ȃ����̃`�F�b�N
	/// </summary>
	/// <returns></returns>
	bool GetInstanceDrawCheck() { return instanceDrawNum < draw_max_num; }

	/// <summary>
	/// �C���X�^���V���O�`���
	/// </summary>
	/// <returns></returns>
	int GetInstanceDrawNum() { return instanceDrawNum; }

private:

	//�f�o�C�X
	static ID3D12Device* device;
	//�R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	//�p�C�v���C��
	static GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline;
	//�J����
	static Camera* camera;
	//���C�g
	static LightGroup* light;
	//�A�E�g���C���̐F
	static XMFLOAT4 outlineColor;
	//�A�E�g���C���̕�
	static float outlineWidth;

private:

	//���f��
	Model* model;
	//�x�[�X�J���[
	OBJECT_INFO objInform;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB0;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB1;
	//�u���[���̗L��
	bool isBloom = false;
	//�g�D�[���̗L��
	bool isToon = false;
	//�A�E�g���C���̗L��
	bool isOutline = false;
	//���C�e�B���O
	bool isLight = true;
	//�C���X�^���V���O�`���
	int instanceDrawNum = 0;

public:

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
	static void SetOutlineColor(const XMFLOAT4& _outlineColor) { InstanceObject::outlineColor = _outlineColor; }

	/// <summary>
	/// �A�E�g���C���̕��Z�b�g
	/// </summary>
	/// <param name="_outlineWidth">��</param>
	static void SetOutlineWidth(float _outlineWidth) { InstanceObject::outlineWidth = _outlineWidth; }

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
	/// ���[���h���W���擾
	/// </summary>
	/// <returns></returns>
	XMFLOAT3 GetWorldPosition();
};