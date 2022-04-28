#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class PmxModel
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

	enum HeaderDataIndex
	{
		ENCODING_FORMAT,
		NUMBER_OF_ADD_UV,
		VERTEX_INDEX_SIZE,
		TEXTURE_INDEX_SIZE,
		MATERIAL_INDEX_SIZE,
		BONE_INDEX_SIZE,
		RIGID_BODY_INDEX_SIZE
	};

	enum WeightType
	{
		BDEF1,
		BDEF2,
		BDEF4,
		SDEF,
	};

	struct Weight
	{
		WeightType type;
		int born1;
		int	born2;
		int	born3;
		int	born4;
		float weight1;
		float weight2;
		float weight3;
		float weight4;
		XMFLOAT3 c;
		XMFLOAT3 r0;
		XMFLOAT3 r1;
	};

	//���_�\����
	struct Vertex
	{
		// ���_���W
		XMFLOAT3 position;
		// �@��
		XMFLOAT3 normal;
		// UV���W
		XMFLOAT2 uv;
		// �ǉ�UV���W
		std::vector<XMFLOAT4> additionalUV;
		// �{�[���E�F�C�g
		Weight weight;
		// �G�b�W�{��
		float edgeMagnif;
	};

	//��
	struct Surface
	{
		int vertexIndex;
	};

	//�}�e���A��
	struct Material
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 specular;
		float specularity;
		XMFLOAT3 ambient;

		int colorMapTextureIndex;
		int toonTextureIndex;
		// (�X�t�B�A�e�N�X�`���͔�Ή�)

		// �ގ����Ƃ̒��_��
		int vertexNum;
	};

	//�{�[��
	struct Bone
	{
		// �{�[����
		std::string name;
		// English version
		std::string nameEnglish;
		XMFLOAT3 position;
		int parentIndex;
		int transformationLevel;
		unsigned short flag;
		XMFLOAT3 coordOffset;
		int childrenIndex;
		int impartParentIndex;
		float impartRate;
		// �Œ莲�����x�N�g��
		XMFLOAT3 fixedAxis;
		// ���[�J����X�������x�N�g��
		XMFLOAT3 localAxisX;
		// ���[�J����Z�������x�N�g��
		XMFLOAT3 localAxisZ;
		int externalParentKey;
		int ikTargetIndex;
		int ikLoopCount;
		float ikUnitAngle;
		struct IKLink
		{
			int index;
			bool existAngleLimited;
			XMFLOAT3 limitAngleMin;
			XMFLOAT3 limitAngleMax;
		};
		std::vector<IKLink> ikLinks;
	};

	// �萔�o�b�t�@�p�f�[�^�\����B1
	static const int B1_size = 64;
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient[B1_size]; // �A���r�G���g�W��
		float pad1; // �p�f�B���O
		XMFLOAT3 diffuse[B1_size]; // �f�B�t���[�Y�W��
		float pad2; // �p�f�B���O
		XMFLOAT3 specular[B1_size]; // �X�y�L�����[�W��
		float alpha[B1_size];	// �A���t�@
	};

private:

	static const std::string baseDirectory;

	static const int NO_DATA_FLAG = -1;
	// �f�o�C�X
	static ID3D12Device* device;

public://�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// �C���X�^���X�̐���
	/// </summary>
	/// <param name="_filePath">���f���t�@�C���p�X</param>
	/// <returns></returns>
	static std::unique_ptr<PmxModel> Create(const std::string& _filePath);

public://�����o�֐�

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	PmxModel() {};

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~PmxModel() {};

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cmdList">���ߔ��s��R�}���h���X�g</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

private://�����o�ϐ�

	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// �e�N�X�`���o�b�t�@
	std::vector<ComPtr<ID3D12Resource>> texBuffer;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB1;
	//���_�z��
	std::vector<Vertex> vertices;
	//��
	std::vector<Surface> surfaces;
	//�e�N�X�`���p�X
	std::vector<std::string> texturePaths;
	//�}�e���A��
	std::vector<Material> materials;
	//�{�[��
	std::vector<Bone> bones;

	private://�����o�֐�

	/// <summary>
	/// ���_�̓ǂݍ���
	/// </summary>
	/// <param name="pmxFile">�t�@�C���f�[�^</param>
	/// <param name="hederData">�t�@�C�����ʃf�[�^</param>
	/// <returns>���_�z��</returns>
		static std::vector<Vertex> CollectVertex(std::ifstream& pmxFile, std::array<byte, 8> hederData);

	/// <summary>
	/// �ʂ̓ǂݍ���
	/// </summary>
	/// <param name="pmxFile">�t�@�C���f�[�^</param>
	/// <param name="hederData">�t�@�C�����ʃf�[�^</param>
	/// <returns>�ʔz��</returns>
	static std::vector<Surface> CollectSurface(std::ifstream& pmxFile, std::array<byte, 8> hederData);

	/// <summary>
	/// �e�N�X�`���̓ǂݍ���
	/// </summary>
	/// <param name="pmxFile">�t�@�C���f�[�^</param>
	/// <param name="folderPath">�t�H���_�p�X</param>
	/// <returns>�e�N�X�`���z��</returns>
	static std::vector<std::string> CollectTexturePaths(std::ifstream& pmxFile, std::string folderPath);

	/// <summary>
	/// �}�e���A���̓ǂݍ���
	/// </summary>
	/// <param name="pmxFile">�t�@�C���f�[�^</param>
	/// <param name="hederData">�t�@�C�����ʃf�[�^</param>
	/// <param name="arrayLength">�z��</param>
	/// <returns>�}�e���A���z��</returns>
	static std::vector<Material> CollectMaterial(std::ifstream& pmxFile,
		std::array<byte, 8> hederData, unsigned arrayLength);

	/// <summary>
	/// �{�[���̓ǂݍ���
	/// </summary>
	/// <param name="pmxFile">�t�@�C���f�[�^</param>
	/// <param name="hederData">�t�@�C�����ʃf�[�^</param>
	/// <param name="arrayLength">�z��</param>
	/// <returns>�{�[���z��</returns>
	static std::vector<Bone> CollectBone(std::ifstream& pmxFile,
		std::array<byte, 8> hederData, unsigned arrayLength);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	void LoadTexture();

public://�����o�֐�

	/// <summary>
	/// PMX���f���f�[�^�̃��[�h�֐�
	/// </summary>
	/// <param name="_filePath">���f���t�@�C���p�X</param>
	/// <param name="vertices">���_�z��</param>
	/// <param name="surfaces">�ʔz��</param>
	/// <param name="texturePaths">�e�N�X�`���z��</param>
	/// <param name="materials">�}�e���A���z��</param>
	/// <param name="bones">�{�[���z��</param>
	static void LoadPMX(const std::string& _filePath, std::vector<Vertex>& vertices,
		std::vector<Surface>& surfaces, std::vector<std::string>& texturePaths,
		std::vector<Material>& materials, std::vector<Bone>& bones);

};