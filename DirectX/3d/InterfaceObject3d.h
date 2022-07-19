#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class BaseCollider;
class Camera;
class LightGroup;
class GraphicsPipelineManager;
class Model;
class Texture;

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

public: // �T�u�N���X

	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0
	{
		XMFLOAT4 color;
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
		XMFLOAT3 baseColor;//�x�[�X�J���\
		float pad1; // �p�f�B���O
		XMFLOAT3 ambient; // �A���r�G���g�W��
		float pad2; // �p�f�B���O
		XMFLOAT3 diffuse; // �f�B�t���[�Y�W��
		float metalness; // �����x(0 or 1)
		float specular; // ���ʔ��˓x
		float roughness; // �e��
		float alpha;	// �A���t�@
		//float pad[3];//�p�f�B���O
	};

public:

	virtual ~InterfaceObject3d() {};

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// �`��O����
	/// </summary>
	virtual void PreDraw() = 0;

	/// <summary>
	/// �`��㏈��
	/// </summary>
	virtual void PostDraw() = 0;

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// ���
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// �f�o�C�X�̃Z�b�g
	/// </summary>
	/// <param name="device">�f�o�C�X��</param>
	void SetDevice(ID3D12Device* device) { InterfaceObject3d::device = device; }

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="camera">�J����</param>
	void SetCamera(Camera* camera) { InterfaceObject3d::camera = camera; }

	/// <summary>
	/// ���C�g�O���[�v�̃Z�b�g
	/// </summary>
	/// <param name="lightGroup">���C�g�O���[�v</param>
	void SetLightGroup(LightGroup* light) { InterfaceObject3d::light = light; }

	/// <summary>
	/// �L���[�u�}�b�v�̃Z�b�g
	/// </summary>
	/// <param name="cubetex">�L���[�u�}�b�v�e�N�X�`��</param>
	void SetLightGroup(Texture* cubetex) { InterfaceObject3d::cubetex = cubetex; }

protected:

	// �f�o�C�X
	ID3D12Device* device = nullptr;
	//�R�}���h���X�g
	ID3D12GraphicsCommandList* cmdList = nullptr;
	//�J����
	Camera* camera = nullptr;
	//���C�g
	LightGroup* light = nullptr;
	//�A�E�g���C���̐F
	XMFLOAT4 outlineColor = {};
	//�A�E�g���C���̕�
	float outlineWidth = 0;
	//�L���[�u�}�b�v
	Texture* cubetex = nullptr;
};