#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class PmxLoader
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

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
		std::wstring name;
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

public://�Œ�l

	static const int NO_DATA_FLAG = -1;

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
	static std::vector<std::wstring> CollectTexturePaths(std::ifstream& pmxFile, std::wstring folderPath);

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
	static void LoadPMX(const std::wstring& _filePath, std::vector<Vertex>& vertices,
		std::vector<Surface>& surfaces, std::vector<std::wstring>& texturePaths,
		std::vector<Material>& materials, std::vector<Bone>& bones);

};

