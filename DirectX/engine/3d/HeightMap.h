#pragma once
#include "InterfaceObject3d.h"

class BaseCollider;
class Camera;
class LightGroup;

class HeightMap : public InterfaceObject3d
{
private: // �G�C���A�X

	struct HEIGHT_MAP_INFO {		// Heightmap structure
		int terrainWidth;		// Width of heightmap
		int terrainHeight;		// Height (Length) of heightmap
		std::vector<XMFLOAT3> heightMap;	// Array to store terrain's vertex positions
	};

	enum TEXTURE {
		HEIGHT_MAP_TEX,
		GRAPHIC_TEX_1,
		GRAPHIC_TEX_2,
		SIZE,
	};

	struct OBJECT_INFO
	{
		float ratio;
	};

public://�����o�֐�

	/// <summary>
	/// directXCommon����
	/// </summary>
	/// <param name="_heightmapFilename">heightmap��</param>
	/// <param name="_filename">�t�@�C����1</param>
	/// <param name="_filename2">�t�@�C����2</param>
	/// <returns>�C���X�^���X</returns>
	static std::unique_ptr<HeightMap> Create(const std::string& _heightmapFilename,
		const std::string& _filename1 = "null", const std::string& _filename2 = "null");

	/// <summary>
	/// �`��O����
	/// </summary>
	static void PreDraw();

private://�����o�֐�

	/// <summary>
	/// �n�C�g�}�b�v
	/// </summary>
	/// <param name="_filename">�t�@�C����</param>
	/// <returns>������</returns>
	bool HeightMapLoad(const std::string& _filename);

	/// <summary
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="_filename1">�t�@�C����1</param>
	/// <param name="_filename2">�t�@�C����2</param>
	void LoadTexture(const std::string& _filename1, const std::string& _filename2);

	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

public:

	HeightMap() {};
	~HeightMap();

	/// <summary>
	/// �ǉ��̒萔�o�b�t�@�̍X�V
	/// </summary>
	void AddConstBufferUpdate(const float _ratio = 1.0f);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// �p�C�v���C���̃Z�b�g
	/// </summary>
	/// <param name="_pipeline">�p�C�v���C��</param>
	static void SetPipeline(const GraphicsPipelineManager::GRAPHICS_PIPELINE& _pipeline) { HeightMap::pipeline = _pipeline; }

private:

	//�p�C�v���C��
	static GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline;
	//��b�t�@�C��
	static const std::string baseDirectory;

private:

	//�e�N�X�`�����
	std::array<std::unique_ptr<Texture>, TEXTURE::SIZE> texture;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBufferOData;
	// ���f��
	Model* model = nullptr;
	//�C���f�b�N�X�̑傫��
	int indexNum = 0;
	//���_��
	int vertNum = 0;
	//�n�C�g�}�b�v�̏��
	HEIGHT_MAP_INFO hmInfo;
	//�����蔻��p���_���
	std::vector<Mesh::VERTEX> hitVertices;
	std::vector<unsigned long> hitIndices;

public:

	Model* GetModel() { return model; }
	std::vector<Mesh::VERTEX>& GetHitVertices() { return hitVertices; }
	std::vector<unsigned long>& GetHitIndices() { return hitIndices; }
};