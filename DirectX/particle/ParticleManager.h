#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include<forward_list>

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
	struct Vertex
	{
		XMFLOAT3 pos; // xyz���W
		float scale;//�X�P�[��
		XMFLOAT4 color;
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		XMMATRIX mat;	// �R�c�ϊ��s��
		XMMATRIX matBillboard;//�r���{�[�h�s��
	};

	//�p�[�e�B�N���ꗱ
	struct Particle
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
		int num_frame = 0;
		//�X�P�[��
		float scale = 1.0f;
		//�����l
		float s_scale = 1.0f;
		//�ŏI�l
		float e_scale = 0.0f;
		//�J���[
		XMFLOAT4 color = { 0,0,0,0 };
		//�����J���[
		XMFLOAT4 s_color = { 0,0,0,0 };
		//�ŏI�J���[
		XMFLOAT4 e_color = { 0,0,0,0 };
	};

private: // �萔
	static const int vertexCount = 512;// ���_��
	std::forward_list<Particle>particle;

public: // �ÓI�����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void Initialize(ID3D12Device* device);

	/// <summary>
	/// �p�C�v���C������
	/// </summary>
	static void Pipeline();

	/// <summary>
	/// �e�N�X�`���f�X�N���v�^�̐���
	/// </summary>
	static void CommonCreate();

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="texNum">�e�N�X�`���ԍ�</param>
	/// <param name="filename">�t�@�C����</param>
	static void LoadTexture(UINT texNum, const wchar_t* filename);

	/// <summary>
	/// �C���X�^���X����
	/// </summary>
	/// <param name="texNumber">�e�N�X�`���ԍ�</param>
	/// <returns>�C���X�^���X</returns>
	static std::unique_ptr<ParticleManager> Create(UINT texNumber);

	/// <summary>
	/// �������
	/// </summary>
	static void AllDelete();

private: // �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device;
	//�R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootSignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelineState;
	// �f�X�N���v�^�q�[�v
	static ComPtr<ID3D12DescriptorHeap> descHeap;
	//�e�N�X�`���ǂݍ��ݍő�l
	static const int textureNum = 50;
	// �e�N�X�`���o�b�t�@
	static ComPtr<ID3D12Resource> texBuffer[textureNum];
	//�r���{�[�h�s��
	static XMMATRIX matBillboard;
	//Y�����̃r���{�[�h�s��
	static XMMATRIX matBillboardY;

private:// �ÓI�����o�֐�

	//�r���[�s����X�V
	static XMMATRIX UpdateViewMatrix(Camera* camera);

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
	/// <param name="maxFrame">��������</param>
	/// <param name="position">�������W</param>
	/// <param name="velocity">���x</param>
	/// <param name="accel">�����x</param>
	/// <param name="startScale">�����T�C�Y</param>
	/// <param name="endScale">�ŏI�T�C�Y</param>
	/// <param name="startColor">�����J���[</param>
	/// <param name="endColor">�ŏI�J���[</param>
	void Add(int maxFrame, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel,
		float startScale, float endScale, XMFLOAT4 startColor, XMFLOAT4 endColor);

	/// <summary>
	/// �X�V
	/// </summary>
	/// <returns>�z��</returns>
	int Update(Camera* camera);

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
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// �o�����Ă���p�[�e�B�N����S�č폜����
	/// </summary>
	void ParticlAllDelete();

private: // �����o�ϐ�

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff = {};
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// ���_�f�[�^�z��
	Vertex vertices[vertexCount] = {};
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	//�e�N�X�`���i���o�[
	int texNumber = 0;

};