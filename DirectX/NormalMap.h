#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class BaseCollider;
class Camera;

class NormalMap
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�\���̐錾
	//�萔�o�b�t�@�̍\����
	struct ConstBufferData {
		XMFLOAT4 color1;//1����
		XMFLOAT4 color2;//2����
		XMFLOAT4 color3;//3����
		XMMATRIX matWorld;//3D�ϊ��s��
		XMMATRIX matView;//3D�ϊ��s��
		XMMATRIX maProjection;//3D�ϊ��s��
		float light;
		float uvPos;
	};

	//���_�f�[�^3D
	struct Vertex {
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

private://�ÓI�����o�֐�

	/// <summary>
	/// �p�C�v���C������
	/// </summary>
	static void Pipeline();

	/// <summary>
	/// �e�N�X�`���f�X�N���v�^�̐���
	/// </summary>
	static void CommonCreate();

public://�ÓI�����o�֐�

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <returns></returns>
	NormalMap() {};

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~NormalMap();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="filename"></param>
	/// <returns>�e�N�X�`���ԍ�</returns>
	static int LoadTexture(const wchar_t* filename);

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

public://�����o�֐�

	/// <summary>
	/// ���̃Z�b�g
	/// </summary>
	/// <param name="position">���W</param>
	/// <param name="rotation">��]�p</param>
	/// <param name="scale">�傫��</param>
	/// <param name="color1">���C���J���[</param>
	/// <param name="color2">�T�u1�J���[</param>
	/// <param name="color3">�T�u2�J���[</param>
	/// <param name="light">���C�g�̈ʒu</param
	/// <param name="uvPos">�e�N�X�`���̈ʒu</param>
	void SetInformation(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale,
		XMFLOAT4 color1, XMFLOAT4 color2, XMFLOAT4 color3, float light, float uvPos)
	{
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
		this->color1 = color1;
		this->color2 = color2;
		this->color3 = color3;
		this->light = light;
		this->uvPos = uvPos;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="camera">�J�����̃C���X�^���X</param>
	void Update(Camera* camera);

	/// <summary>
	/// Object�̐���
	/// </summary>
	void Create();

	/// <summary>
	/// //�`��
	/// </summary>
	/// <param name="colorTex">���C���e�N�X�`��</param>
	/// <param name="normalTex">�@���e�N�X�`��1</param>
	/// <param name="normalTex2">�@���e�N�X�`��2</param>
	void Draw(int colorTex, int normalTex, int normalTex2);


private://�ÓI�����o�ϐ�

	//�f�o�C�X
	static ID3D12Device* device;
	//�R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState>pipelinestate;
	//���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature>rootsignature;
	//�e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
	static ComPtr<ID3D12DescriptorHeap>descHeap;
	//�e�N�X�`���ő�o�^��
	static const int textureNum = 512;
	//�e�N�X�`�����\�[�X(�e�N�X�`���o�b�t�@)�̔z��
	static ComPtr<ID3D12Resource>texBuffer[textureNum];
	//��ӂ̒���
	static const int edge = 5;
	//���_�f�[�^
	static Vertex vertices[24];
	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	static const UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * _countof(vertices));
	//�C���f�b�N�X�f�[�^
	static const unsigned short indices[36];
	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * _countof(indices));
	//�e�N�X�`���̍��v
	static int alltextureNum;

private://�����o�ϐ�

	//�e�N�X�`���ԍ�
	UINT texNumber = -1;
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;
	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView{};
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	// �F1
	XMFLOAT4 color1 = {};
	// �F2
	XMFLOAT4 color2 = {};
	// �F3
	XMFLOAT4 color3 = {};
	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z�����̃��[�J����]�p
	XMFLOAT3 rotation = { 0,0,0 };
	// ���[�J�����W
	XMFLOAT3 position = { 0,0,0 };
	//�e�N�X�`���̏㉺���W
	XMFLOAT2 texLeftTop = { 0,0 };
	//�e�N�X�`���؂�o���T�C�Y
	XMFLOAT2 texSise = { 500,500 };
	//UV�ʒu
	float uvPos = 0;
	//���̈ʒu
	float light = 0;
};