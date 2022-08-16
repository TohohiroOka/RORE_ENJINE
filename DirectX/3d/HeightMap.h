#pragma once
#include "InterfaceObject3d.h"

class BaseCollider;
class Camera;
class LightGroup;

class HeightMap : public InterfaceObject3d
{
	struct HeightMapInfo {		// Heightmap structure
		int terrainWidth;		// Width of heightmap
		int terrainHeight;		// Height (Length) of heightmap
		std::vector<XMFLOAT3> heightMap;	// Array to store terrain's vertex positions
	};

	enum TEXTURE {
		HeightMapTex,
		GraphicTex,
		Size,
	};

public://�����o�֐�

	/// <summary>
	/// directXCommon����
	/// </summary>
	/// <param name="heightmapFilename">heightmap��</param>
	/// <param name="filename">�t�@�C����</param>
	/// <returns>�C���X�^���X</returns>
	static std::unique_ptr<HeightMap> Create(const std::string heightmapFilename, const std::string filename = "null");

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void PreDraw();

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
	/// <param name="filename">�t�@�C����</param>
	void LoadTexture(const std::string filename);

	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

public:

	HeightMap() {};
	~HeightMap();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// �p�C�v���C���̐ݒ�
	/// </summary>
	/// <param name="pipeline"></param>
	static void SetPipeline(GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline) { HeightMap::pipeline = pipeline; }

private:

	//�p�C�v���C��
	static GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline;
	//��b�t�@�C��
	static const std::string baseDirectory;

private:
	
	//�e�N�X�`�����
	std::array<std::unique_ptr<Texture>,2> texture;
	// ���f��
	Model* model = nullptr;
	//�C���f�b�N�X�̑傫��
	int indexNum = 0;
	//���_��
	int vertNum = 0;
	//�n�C�g�}�b�v�̏��
	HeightMapInfo hmInfo;

public:

	Model* GetModel() { return model; }

};