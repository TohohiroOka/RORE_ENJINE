#pragma once
#include<fbxsdk.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include<DirectXTex.h>
#include<map>

class Camera;

class Fbx
{
protected:// �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�Œ�l
	//�{�[���̍ő�l
	static const int MAX_BONES = 32;

private://�\���̐錾
	static const int MAX_BONE_INDICES = 4;
	//���_�f�[�^3D
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
		UINT boneIndex[MAX_BONE_INDICES];
		float boneWhight[MAX_BONE_INDICES];
	};

	//�m�[�h
	struct Node
	{
		//���O
		std::string name;
		//���[�J���X�P�[��
		DirectX::XMVECTOR scaling = { 1.0f,1.0f,1.0f,0.0f };
		//���[�J����]
		DirectX::XMVECTOR rotation = { 0.0f,0.0f,0.0f,0.0f };
		//���[�J���ړ�
		DirectX::XMVECTOR translation = { 0.0f,0.0f,0.0f,1.0f };
		//���[�J���ό`�s��
		DirectX::XMMATRIX transform;
		//�O���[�o���ό`�s��
		DirectX::XMMATRIX globalTransform;
		//�e�m�[�h
		Node* parent = nullptr;
	};

	//�{�[��
	struct Bone
	{
		//���O
		std::string name;

		//�����p���s��
		DirectX::XMMATRIX invInitialPose;

		//�N���X�^�[
		FbxCluster* fbxCluster;

		//�R���X�g���N�^
		Bone(const std::string& name)
		{
			this->name = name;
		}
	};

	//�}�e���A���p
	struct Material
	{
		std::string name;//�}�e���A����
		XMFLOAT3 ambient;//�A���r�G���g�e���x
		XMFLOAT3 diffuse;//�f�B�t���[�Y�e���x
		XMFLOAT3 emissive;// �G�~�b�V�u
		XMFLOAT3 bump;// �o���v
		XMFLOAT3 specular;//�X�y�L�����e���x
		float alpha;//�A���t�@
		std::string textureFilename;//�e�N�X�`���t�@�C����
		//�R���X�g���N�^
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.3f,0.3f,0.3f };
			specular = { 0.3f,0.3f,0.3f };
			alpha = 1.0f;
		}
	};

	//�萔�o�b�t�@�p�f�[�^B0
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;//�r���[�v���W�F�N�V�����s��
		XMMATRIX world;//���[���h�s��
		XMFLOAT3 cameraPos;//�J�������W�i���[���h���W�j
		XMFLOAT4 color;//�F
	};

	//�X�L���p�萔�o�b�t�@�f�[�^
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

	struct FbxUpdate
	{
		bool isAnimation = false;//�A�j���[�V�����\��
		FbxScene* fbxScene = nullptr;
		FbxTime startTime;//�t���[���̃X�^�[�g
		FbxTime stopTime;;//�t���[���̍Ō�
		FbxTime nowTime;//���݂̐i�s�t���[��
	};

	//Fbx�f�[�^
	struct Data
	{
		Material material;
		std::vector<Vertex> vertices;
		std::vector<unsigned short>indices;
		std::vector<Node> nodes;
		std::vector<Bone> bones;
		FbxUpdate fbxUpdate;
	};

private://�ÓI�����o�֐��֐�

	/// <summary>
	/// �p�C�v���C���ݒ�
	/// </summary>
	static void Pipeline();

	/// <summary>
	/// �m�[�h�ǂݍ���
	/// </summary>
	/// <param name="dataNumber">�i�[����z��ԍ�</param>
	/// <param name="fbxNode">�m�[�h</param>
	/// <param name="parent">�e�m�[�h</param>
	static void LoadNode(int dataNumber, FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// ���b�V����T��
	/// </summary>
	/// <param name="dataNumber">�i�[����z��ԍ�</param>
	/// <param name="fbxNode">�m�[�h</param>
	static void CollectMesh(int dataNumber, FbxNode* fbxNode);

	/// <summary>
	/// ���_�ǂݍ���
	/// </summary>
	/// <param name="dataNumber">�i�[����z��ԍ�</param>
	/// <param name="fbxMesh">���b�V��</param>
	static void CollectVertices(int dataNumber, FbxMesh* fbxMesh);

	/// <summary>
	/// �ʂ��Ƃ̓ǂݍ���
	/// </summary>
	/// <param name="dataNumber">�i�[����z��ԍ�</param>
	/// <param name="fbxMesh">���b�V��</param>
	static void CollectMeshFaces(int dataNumber, FbxMesh* fbxMesh);

	/// <summary>
	/// �X�L�j���O���̓ǂݎ��
	/// </summary>
	/// <param name="dataNumber">�i�[����z��ԍ�</param>
	/// <param name="fbxMesh">���b�V��</param>
	static void CollectSkin(int dataNumber, FbxMesh* fbxMesh);

	/// <summary>
	/// �}�e���A���ǂݍ���
	/// </summary>
	/// <param name="dataNumber">�i�[����z��ԍ�</param>
	/// <param name="fbxNode">�m�[�h</param>
	static void LoadMaterial(int dataNumber, FbxNode* fbxNode);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="dataNumber">�i�[����z��ԍ�</param>
	/// <param name="fileName">�t�@�C����</param>
	static void LoadTexture(int dataNumber, const std::string fileName);

	/// <summary>
	/// �A�j���[�V�����ǂݍ���
	/// </summary>
	/// <param name="dataNumber">�i�[����z��ԍ�</param>
	static void LoadAnimation(int dataNumber);

	/// <summary>
	/// //�s��̕ϊ�
	/// </summary>
	/// <param name="dst">�i�[����XMMATRIX�^�ϐ�</param>
	/// <param name="src">�ϊ�����FbxMatrix�^�ϐ�</param>
	static void ConvertMatrixFormFbx(DirectX::XMMATRIX* dst, const FbxMatrix& src);

	/// <summary>
	/// //�t�@�C�������o
	/// </summary>
	/// <param name="path">�t�@�C���p�X</param>
	/// <returns></returns>
	static std::string ExtractFileName(const std::string& path);

public://�ÓI�����o�֐�
	
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <returns></returns>
	Fbx() {};

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Fbx();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// Fbx�t�@�C���̓ǂݍ���
	/// </summary>
	/// <param name="fileName">�t�@�C����</param>
	/// <returns>�ۑ��ԍ�</returns>
	static int LoadFbx(const std::string fileName);

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

public:

	/// <summary>
	/// �i�[����Ă���FBX�f�[�^�����Ƀf�[�^���쐬����
	/// </summary>
	/// <param name="modelNumber">�ۑ��ԍ�</param>
	void CreateModel(UINT modelNumber);

	/// <summary>
	/// ����
	/// </summary>
	void Create();

	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="camera">�J�����̃C���X�^���X</param>
	void Update(Camera* camera);

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="modelNumber"></param>
	void Draw(int modelNumber);

	/// <summary>
	/// ���̃Z�b�g
	/// </summary>
	/// <param name="position"></param>
	/// <param name="rotation"></param>
	/// <param name="scale"></param>
	/// <param name="isAnimation"></param
	void SetInformation(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, bool isAnimation) {
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
		this->isAnimation = isAnimation;
	}

	//���[���h�s��̎擾
	const XMMATRIX& GetMatWorld() { return matWorld; }


private://�ÓI�����o�ϐ�
	static FbxManager* fbxManager;//Fbx�̊��
	static FbxTime frameTime;//1�t���[���̎���
	static ID3D12Device* device;//�f�o�C�X
	static ID3D12GraphicsCommandList* cmdList;//�R�}���h���X�g
	static std::vector<Data> data;//Fbx�f�[�^�̊i�[�ꏊ
	static ComPtr<ID3D12PipelineState>pipelinestate;//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12RootSignature>rootsignature;//���[�g�V�O�l�`��
	static ComPtr<ID3D12DescriptorHeap>descHeap;//�e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
	static const int textureNum = 512;//�e�N�X�`���ő�o�^��
	static ComPtr<ID3D12Resource>texBuffer[textureNum];//�e�N�X�`�����\�[�X(�e�N�X�`���o�b�t�@)�̔z��
	static int vecSize;//���݂̔z��
	static const std::string subTexture;//texture����������texture
	static const std::string directoryPath;//�t�@�C���p�X
	static std::string fileName;//�t�@�C���l�[���̕ێ�

private://���I�����o�ϐ�
	UINT modelNumber = -1;//�e�N�X�`���ԍ�
	ComPtr<ID3D12Resource> constBuffTransform;//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffSkin;//�萔�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;//���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView;//���_�o�b�t�@�r���[
	ComPtr<ID3D12Resource> indexBuff;//�C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW ibView;//�C���f�b�N�X�o�b�t�@�r���[
	XMMATRIX matWorld;// ���[�J�����[���h�ϊ��s��
	XMFLOAT3 position = {};//���W
	XMFLOAT3 rotation = {};//��]�p
	XMFLOAT3 scale = {};//�傫��
	bool isAnimation = false;//�A�j���[�V�������邩
};