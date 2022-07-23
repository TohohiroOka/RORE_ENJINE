#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include "GraphicsPipelineManager.h"
#include "Texture.h"
#include "LightGroup.h"
#include <array>
class Camera;

class HeightMap
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
	struct Vertex {
		XMFLOAT3 pos; // xyz���W
		XMFLOAT3 normal; // �@���x�N�g��
		XMFLOAT2 uv;  // uv���W
	};

	//�萔�o�b�t�@�̍\����
	struct ConstBufferData {
		XMMATRIX viewproj; // �r���[�v���W�F�N�V�����s��
		XMMATRIX world; // ���[���h�s��
		XMFLOAT3 cameraPos; // �J�������W�i���[���h���W�j
	};

	struct HeightMapInfo {		// Heightmap structure
		int terrainWidth;		// Width of heightmap
		int terrainHeight;		// Height (Length) of heightmap
		std::vector<XMFLOAT3> heightMap;	// Array to store terrain's vertex positions
	};

	enum TEXTURE {
		HeightMapTex,
		GraphicTex1,
		GraphicTex2,
		Size,
	};

private:

	/// <summary>
	/// �p�C�v���C������
	/// </summary>
	static void CreateGraphicsPipeline();

public://�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// directXCommon����
	/// </summary>
	/// <param name="heightmapFilename">heightmap��</param>
	/// <param name="filename">�t�@�C����</param>
	/// <returns>�C���X�^���X</returns>
	static std::unique_ptr<HeightMap> Create(const std::string heightmapFilename,
		const std::string filename1 = "null", const std::string filename2 = "null");

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="camera">�J����</param>
	static void SetCamera(Camera* camera) {
		HeightMap::camera = camera;
	}

	/// <summary>
	/// ���C�g�O���[�v�̃Z�b�g
	/// </summary>
	/// <param name="lightGroup">���C�g�O���[�v</param>
	static void SetLightGroup(LightGroup* lightGroup) {
		HeightMap::lightGroup = lightGroup;
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
	/// �������
	/// </summary>
	static void Finalize();

private://�����o�֐�

	/// <summary>
	/// �n�C�g�}�b�v
	/// </summary>
	/// <param name="filename">�t�@�C����</param>
	/// <returns>������</returns>
	bool HeightMapLoad(const std::string filename);

	/// <summary
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="filename1">�t�@�C����</param>
	/// <param name="filename2">�t�@�C����</param>
	void LoadTexture(const std::string filename1, const std::string filename2);

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

public:

	HeightMap() {};
	~HeightMap();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(UINT topology);

private:

	//�f�o�C�X
	static ID3D12Device* device;
	//�R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	//�p�C�v���C��
	static std::unique_ptr<GraphicsPipelineManager> pipeline;
	//�J����
	static Camera* camera;
	// ���C�g
	static LightGroup* lightGroup;
	//��b�t�@�C��
	static const std::string baseDirectory;

private:
	
	//�e�N�X�`�����
	std::array<std::unique_ptr<Texture>, TEXTURE::Size> texture;
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
	//�傫��
	XMFLOAT3 scale = { 25,25,25 };
	//�C���f�b�N�X�̑傫��
	int indexNum = 0;
	//���_��
	int vertNum = 0;
	HeightMapInfo hmInfo;
};