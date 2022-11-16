#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include<forward_list>

#include "GraphicsPipelineManager.h"
#include "Texture.h"

class Camera;

class ParticleManager
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X
	// ���_�f�[�^�\����
	struct VERTEX
	{
		XMFLOAT3 pos; // xyz���W
		float scale;//�X�P�[��
		XMFLOAT4 color;
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct CONST_BUFFER_DATA
	{
		XMMATRIX mat;	// �R�c�ϊ��s��
		XMMATRIX matBillboard;//�r���{�[�h�s��
		unsigned int isBloom;
	};

	//�p�[�e�B�N���ꗱ
	struct PARTICLE
	{
		//DirectX::���ȗ�
		using XMFLOAT3 = DirectX::XMFLOAT3;

		//���W
		XMFLOAT3 position = {};
		//���x
		XMFLOAT3 velocity = {};
		//�����x
		XMFLOAT3 accel = {};
		//���݃t���[��
		int frame = 0;
		//�I���t���[��
		int numFrame = 0;
		//�X�P�[��
		float scale = 1.0f;
		//�����l
		float startScale = 1.0f;
		//�ŏI�l
		float endScale = 0.0f;
		//�J���[
		XMFLOAT4 color = { 0,0,0,0 };
		//�����J���[
		XMFLOAT4 startColor = { 0,0,0,0 };
		//�ŏI�J���[
		XMFLOAT4 endColor = { 0,0,0,0 };
	};

	struct INFORMATION
	{
		bool isDelete = false; //�V�[���J�ڂō폜���s����
		std::unique_ptr<Texture> instance = nullptr;
	};

private: // �萔

	static const int vertexCount = 512;// ���_��
	std::forward_list<PARTICLE>particle;

public: // �ÓI�����o�֐�

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="_keepName">�ۑ���</param>
	/// <param name="_filename">�t�@�C����</param>
	/// <param name="_isDelete">�V�[���J�ڂō폜���s����</param>
	static void LoadTexture(const std::string& _keepName, const std::string& _filename, bool _isDelete = true);

	/// <summary>
	/// �C���X�^���X����
	/// </summary>
	/// <param name="_name">�e�N�X�`����</param>
	/// <returns>�C���X�^���X</returns>
	static std::unique_ptr<ParticleManager> Create(const std::string& _name);

	/// <summary>
	/// �f�o�C�X�̃Z�b�g
	/// </summary>
	/// <param name="_device">�f�o�C�X</param>
	static void SetDevice(ID3D12Device* _device) {
		ParticleManager::device = _device;
	}

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="_camera">�J����</param>
	static void SetCamera(Camera* _camera) {
		ParticleManager::camera = _camera;
	}

	/// <summary>
	/// �V�[�����Ƃ̉������
	/// </summary>
	static void SceneFinalize();

	/// <summary>
	/// �������
	/// </summary>
	static void Finalize();

	/// <summary>
	/// �p�C�v���C���̃Z�b�g
	/// </summary>
	/// <param name="pipeline">�p�C�v���C��</param>
	static void SetPipeline(const GraphicsPipelineManager::GRAPHICS_PIPELINE& _pipeline) { pipeline = _pipeline; }

private: // �ÓI�����o�ϐ�

	// �f�o�C�X
	static ID3D12Device* device;
	//�R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	//�J����
	static Camera* camera;
	//�p�C�v���C��
	static GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline;
	//�e�N�X�`�����
	static std::map<std::string, INFORMATION> texture;
	//�r���{�[�h�s��
	static XMMATRIX matBillboard;
	//Y�����̃r���{�[�h�s��
	static XMMATRIX matBillboardY;

private:// �ÓI�����o�֐�

	/// <summary>
	/// �r���[�s��̍X�V
	/// </summary>
	/// <returns>�r���[�s��</returns>
	static XMMATRIX UpdateViewMatrix();

public: // �����o�֐�

	ParticleManager() {};
	~ParticleManager();

	/// <summary>
	/// �p�[�e�B�N���̐���
	/// </summary>
	void Initialize();

	/// <summary>
	/// �p�[�e�B�N���̒ǉ�
	/// </summary>
	/// <param name="_maxFrame">��������</param>
	/// <param name="_position">�������W</param>
	/// <param name="_velocity">���x</param>
	/// <param name="_accel">�����x</param>
	/// <param name="_startScale">�����T�C�Y</param>
	/// <param name="_endScale">�ŏI�T�C�Y</param>
	/// <param name="_startColor">�����J���[</param>
	/// <param name="_endColor">�ŏI�J���[</param>
	void Add(int _maxFrame, const XMFLOAT3& _position, const XMFLOAT3& _velocity, const XMFLOAT3& _accel,
		float _startScale, float _endScale, const XMFLOAT4& _startColor, const XMFLOAT4& _endColor);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="_cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* _cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// �o�����Ă���p�[�e�B�N����S�č폜����
	/// </summary>
	void ParticlAllDelete();

	/// <summary>
	/// ���݂̐�
	/// </summary>
	int GetCreateNum() {
		return int(std::distance(particle.begin(), particle.end()));
	}

private: // �����o�ϐ�

	//�e�N�X�`����
	std::string name;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff = {};
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// ���_�f�[�^�z��
	VERTEX vertices[vertexCount] = {};
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	//�u���[���̗L��
	bool isBloom = false;

public:

	/// <summary>
	/// �u���[���̃Z�b�g
	/// </summary>
	/// <param name="isBloom">�u���[���L->true / ��->false</param>
	void SetBloom(bool isBloom) { this->isBloom = isBloom; }
};