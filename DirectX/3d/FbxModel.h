#pragma once
#include <fbxsdk.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <map>

class FbxModel
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

public://�Œ�l

	//�{�[���̍ő�l
	static const int MAX_BONES = 32;
	//�e�N�X�`���ő�o�^��
	static const int textureNum = 256;

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
		XMVECTOR scaling = { 1.0f,1.0f,1.0f,0.0f };
		//���[�J����]
		XMVECTOR rotation = { 0.0f,0.0f,0.0f,0.0f };
		//���[�J���ړ�
		XMVECTOR translation = { 0.0f,0.0f,0.0f,1.0f };
		//���[�J���ό`�s��
		XMMATRIX transform = {};
		//�O���[�o���ό`�s��
		XMMATRIX globalTransform = {};
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
		XMFLOAT3 ambient = {};//�A���r�G���g�e���x
		XMFLOAT3 diffuse = {};//�f�B�t���[�Y�e���x
		float alpha = 0;//�A���t�@
		XMFLOAT3 baseColor = { 1,1,1 };//�x�[�X�J���\
		float metalness = 0.0f;//�����x(0 or 1)
		float specular = 0.5f;//���ʔ��˓x
		float roughness = 0.0f;//�e��
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

	//�X�L���p�萔�o�b�t�@�f�[�^
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

	//�A�j���[�V�����f�[�^
	struct FbxUpdate
	{
		bool isAnimation = false;//�A�j���[�V�����\��
		FbxScene* fbxScene = nullptr;
		FbxTime startTime = {};//�t���[���̃X�^�[�g
		FbxTime stopTime = {};//�t���[���̍Ō�
		FbxTime nowTime = {};//���݂̐i�s�t���[��
	};

	//Fbx�f�[�^
	struct Data
	{
		Material material;
		std::vector<Vertex> vertices;
		std::vector<unsigned short>indices;
		std::vector<Node> nodes;
		Node* meshNode;
		std::vector<Bone> bones;
		FbxUpdate fbxUpdate;
	};

private://�����o�֐�

	/// <summary>
	/// �m�[�h�ǂݍ���
	/// </summary>
	/// <param name="parent">�e�m�[�h</param>
	void LoadNode(FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// ���b�V����T��
	/// </summary>
	void CollectMesh(FbxNode* fbxNode);

	/// <summary>
	/// ���_�ǂݍ���
	/// </summary>
	/// <param name="fbxMesh">���b�V��</param>
	void CollectVertices(FbxMesh* fbxMesh);

	/// <summary>
	/// �ʂ��Ƃ̓ǂݍ���
	/// </summary>
	/// <param name="fbxMesh">���b�V��</param>
	void CollectMeshFaces(FbxMesh* fbxMesh);

	/// <summary>
	/// �X�L�j���O���̓ǂݎ��
	/// </summary>
	/// <param name="fbxMesh">���b�V��</param>
	void CollectSkin(FbxMesh* fbxMesh);

	/// <summary>
	/// �}�e���A���ǂݍ���
	/// </summary>
	void LoadMaterial(FbxNode* fbxNode);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="dataNumber">�i�[����z��ԍ�</param>
	/// <param name="fileName">�t�@�C����</param>
	void LoadTexture(const std::string fileName);

	/// <summary>
	/// �A�j���[�V�����ǂݍ���
	/// </summary>
	void LoadAnimation(FbxScene* fbxScene);

	/// <summary>
	/// Fbx�t�@�C���̓ǂݍ���
	/// </summary>
	/// <param name="fileName">�t�@�C����</param>
	void LoadFbx(const std::string modelName);

	/// <summary>
	/// ����
	/// </summary>
	void Initialize();

	/// <summary>
	/// //�s��̕ϊ�
	/// </summary>
	/// <param name="dst">�i�[����XMMATRIX�^�ϐ�</param>
	/// <param name="src">�ϊ�����FbxMatrix�^�ϐ�</param>
	void ConvertMatrixFormFbx(DirectX::XMMATRIX* dst, const FbxMatrix& src);

	/// <summary>
	/// //�t�@�C�������o
	/// </summary>
	/// <param name="path">�t�@�C���p�X</param>
	/// <returns></returns>
	std::string ExtractFileName(const std::string& path);

public://�ÓI�����o�֐�

	FbxModel() {};
	~FbxModel();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// �C���X�^���X�̐���
	/// </summary>
	/// <param name="fileName">�t�@�C����</param>
	static std::unique_ptr<FbxModel> Create(const std::string fileName);

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	
	/// <summary>
	/// �������
	/// </summary>
	static void Finalize();

public:

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param
	void Draw(ID3D12GraphicsCommandList* cmdList);

private://�ÓI�����o�ϐ�

	//�f�o�C�X
	static ID3D12Device* device;
	//Fbx�̊��
	static FbxManager* fbxManager;
	//FBX�C���|�[�^
	static FbxImporter* fbxImporter;
	//1�t���[���̎���
	static FbxTime frameTime;
	//texture����������texture
	static const std::string defaultTexture;
	//�t�@�C���p�X
	static const std::string baseDirectory;
	//�e�N�X�`���p�f�X�N���v�^�q�[�v
	static ComPtr<ID3D12DescriptorHeap>descHeap;
	//�e�N�X�`�����\�[�X(�e�N�X�`���o�b�t�@)�̔z��
	static ComPtr<ID3D12Resource>texBuffer[textureNum];
	//Fbx�̃f�[�^
	std::unique_ptr<Data> data = nullptr;

public://�����o�ϐ�

	//���f����
	std::string name;
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB1;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffSkin = nullptr;
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;
	//�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView;
	//�A�j���[�V�����\��
	bool isAnimation = false;
	//�X�L�j���O���s����
	bool isSkinning = true;

public:

	/// <summary>
	/// �x�[�X�J���[�擾
	/// </summary>
	/// <returns>�x�[�X�J���[</returns>
	const XMFLOAT3& GetBaseColor() { return data->material.baseColor; }

	/// <summary>
	/// �����x�擾
	/// </summary>
	/// <returns>�����x</returns>
	float GetMetalness() { return data->material.metalness; }

	/// <summary>
	/// ���ʔ��˓x�擾
	/// </summary>
	/// <returns>�����x</returns>
	float GetSpecular() { return data->material.specular; }

	/// <summary>
	/// �����x�擾
	/// </summary>
	/// <returns>�����x</returns>
	float GetRoughness() { return data->material.roughness; }

	/// <summary>
	/// �x�[�X�J���[�Z�b�g
	/// </summary>
	void SetBaseColor(const XMFLOAT3& baseColor) { data->material.baseColor = baseColor; }

	/// <summary>
	/// �����x�Z�b�g
	/// </summary>
	void SetMetalness(float metalness) { data->material.metalness = metalness; }

	/// <summary>
	/// ���ʔ��˓x�Z�b�g
	/// </summary>
	void SetSpecular(float specular) { data->material.specular = specular; }

	/// <summary>
	/// �����x�Z�b�g
	/// </summary>
	void SetRoughness(float roughness) { data->material.roughness = roughness; }
};
