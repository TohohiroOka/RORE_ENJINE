#include "TerrainModel.h"
#include <DirectXTex.h>

using namespace DirectX;

const std::string TerrainModel::baseDirectory = "Resources/HeightMap/";

std::unique_ptr<TerrainModel> TerrainModel::FlatlandModelCreate(const FACE_DIRECTION _direction,
	const XMFLOAT3& _pos, const float _scale, const std::string& _filename1, const std::string& _filename2)
{
	//�C���X�^���X�𐶐�
	TerrainModel* instance = new TerrainModel();
	if (instance == nullptr) {
		return nullptr;
	}

	//�e�N�X�`���ǂݍ���
	instance->LoadTexture(_filename1, _filename2);

	//���ʂ̐���
	instance->FlatlandMap(_direction, _pos, _scale);

	return std::unique_ptr<TerrainModel>(instance);
}

std::unique_ptr<TerrainModel> TerrainModel::Create(const std::string& _heightmapFilename, const float _heightRate,
	const FACE_DIRECTION _direction, const XMFLOAT3& _pos, const float _scale, const std::string& _filename1, const std::string& _filename2)
{
	//�C���X�^���X�𐶐�
	TerrainModel* instance = new TerrainModel();
	if (instance == nullptr) {
		return nullptr;
	}

	//�n�`�p�e�N�X�`���ǂݍ���
	instance->HeightMapLoad(_direction, _heightRate, _pos, _scale, _heightmapFilename);

	//�e�N�X�`���ǂݍ���
	instance->LoadTexture(_filename1, _filename2);

	//���f������
	instance->CreaetModel(_direction);

	return std::unique_ptr<TerrainModel>(instance);
}

void TerrainModel::Draw(ID3D12GraphicsCommandList* _cmdList)
{
	model->VIDraw(_cmdList);

	//�e�N�X�`���]��
	_cmdList->SetGraphicsRootDescriptorTable(4, texture[TEXTURE::HEIGHT_MAP_TEX]->descriptor->gpu);
	_cmdList->SetGraphicsRootDescriptorTable(5, texture[TEXTURE::GRAPHIC_TEX_1]->descriptor->gpu);
	_cmdList->SetGraphicsRootDescriptorTable(6, texture[TEXTURE::GRAPHIC_TEX_2]->descriptor->gpu);

	//�`��R�}���h
	_cmdList->DrawIndexedInstanced(indexNum, 1, 0, 0, 0);
}

bool TerrainModel::FlatlandMap(const FACE_DIRECTION _direction, const XMFLOAT3& _pos, const float _scale)
{
	//���ʗp�e�N�X�`��
	std::string flatlandFilename = "heightmap0.bmp";
	texture[TEXTURE::HEIGHT_MAP_TEX] = Texture::Create(baseDirectory + flatlandFilename);

	hmInfo.terrainWidth = 256;
	hmInfo.terrainHeight = 256;

	size_t resize = hmInfo.terrainWidth * hmInfo.terrainHeight;
	hmInfo.heightMap.resize(resize);

	//���W�ړ�����
	transformPos.x = _pos.x / _scale;
	transformPos.y = _pos.y / _scale;
	transformPos.z = _pos.z / _scale;

	// �摜�f�[�^�̓ǂݍ���
	for (int z = 0; z < hmInfo.terrainHeight; z++)
	{
		for (int x = 0; x < hmInfo.terrainWidth; x++)
		{
			int index = (hmInfo.terrainHeight * z) + x;

			hmInfo.heightMap[index].x = transformPos.x;
			hmInfo.heightMap[index].y = transformPos.y;
			hmInfo.heightMap[index].z = transformPos.z;

			if (_direction == FACE_DIRECTION::X_PLUS)
			{
				hmInfo.heightMap[index].x += 0.0f;
				hmInfo.heightMap[index].y += float(z);
				hmInfo.heightMap[index].z += float(x);
			} else if (_direction == FACE_DIRECTION::X_MINUS) {
				hmInfo.heightMap[index].x += 0.0f;
				hmInfo.heightMap[index].y += float(z);
				hmInfo.heightMap[index].z += -float(x);
			} else if (_direction == FACE_DIRECTION::Y_PLUS) {
				hmInfo.heightMap[index].x += float(x);
				hmInfo.heightMap[index].y += 0.0f;
				hmInfo.heightMap[index].z += float(z);
			} else if (_direction == FACE_DIRECTION::Y_MINUS) {
				hmInfo.heightMap[index].x += float(x);
				hmInfo.heightMap[index].y += 0.0f;
				hmInfo.heightMap[index].z += -float(z);
			} else if (_direction == FACE_DIRECTION::Z_PLUS) {
				hmInfo.heightMap[index].x += -float(x);
				hmInfo.heightMap[index].y += float(z);
				hmInfo.heightMap[index].z += 0.0f;
			} else if (_direction == FACE_DIRECTION::Z_MINUS) {
				hmInfo.heightMap[index].x += float(x);
				hmInfo.heightMap[index].y += float(z);
				hmInfo.heightMap[index].z += 0.0f;
			}
		}
	}

	CreaetModel(_direction);

	return true;
}

bool TerrainModel::HeightMapLoad(const FACE_DIRECTION _direction, const float _heightRate,
	const XMFLOAT3& _pos, const float _scale, const std::string& _filename)
{
	//���O����
	std::string fname = baseDirectory + _filename;

	texture[TEXTURE::HEIGHT_MAP_TEX] = Texture::Create(fname);

	//height map���J��
	FILE* filePtr;
	int error = fopen_s(&filePtr, fname.c_str(), "rb");
	if (error != 0)
	{
		return false;
	}

	//�t�@�C���w�b�_�[�̓ǂݍ���
	BITMAPFILEHEADER bitmapFileHeader;
	size_t count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//�r�b�g�}�b�v���w�b�_�[�̓ǂݍ���
	BITMAPINFOHEADER bitmapInfoHeader;
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//�n�`�̏c������ۑ�
	hmInfo.terrainWidth = bitmapInfoHeader.biWidth;
	hmInfo.terrainHeight = bitmapInfoHeader.biHeight;

	//�r�b�g�}�b�v�摜�f�[�^�̃T�C�Y���v�Z
	int imageSize = hmInfo.terrainWidth * hmInfo.terrainHeight * 3;

	//�r�b�g�}�b�v�摜�f�[�^�Ƀ����������蓖��
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	//�r�b�g�}�b�v�f�[�^�̐擪�Ɉړ�
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//�r�b�g�}�b�v�摜�f�[�^�̓ǂݍ���
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	//�t�@�C�������
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//�ۑ��p�z����}�b�v�̑傫���ɕύX
	size_t resize = hmInfo.terrainWidth * hmInfo.terrainHeight;
	hmInfo.heightMap.resize(resize);

	//�ԗv�f�̂ݎg�p���邽�߂̕ϐ�
	int k = 0;

	//�n�`�����傤�ǂ悭���邽�߂Ɋ���l
	float heightFactor = 10.0f;

	//���W�ړ�����
	transformPos.x = _pos.x / _scale;
	transformPos.y = _pos.y / _scale;
	transformPos.z = _pos.z / _scale;

	// �摜�f�[�^�̓ǂݍ���
	for (int z = 0; z < hmInfo.terrainHeight; z++)
	{
		for (int x = 0; x < hmInfo.terrainWidth; x++)
		{
			float height = static_cast<float>(bitmapImage[k]);
			height = height / _heightRate;

			int index = (hmInfo.terrainHeight * z) + x;

			hmInfo.heightMap[index].x = transformPos.x;
			hmInfo.heightMap[index].y = transformPos.y;
			hmInfo.heightMap[index].z = transformPos.z;

			if (_direction == FACE_DIRECTION::X_PLUS)
			{
				hmInfo.heightMap[index].x += float(height);
				hmInfo.heightMap[index].y += float(z);
				hmInfo.heightMap[index].z += float(x);
			} else if (_direction == FACE_DIRECTION::X_MINUS) {
				hmInfo.heightMap[index].x += -float(height);
				hmInfo.heightMap[index].y += float(z);
				hmInfo.heightMap[index].z += -float(x);
			} else if (_direction == FACE_DIRECTION::Y_PLUS) {
				hmInfo.heightMap[index].x += float(x);
				hmInfo.heightMap[index].y += float(height);
				hmInfo.heightMap[index].z += float(z);
			} else if (_direction == FACE_DIRECTION::Y_MINUS) {
				hmInfo.heightMap[index].x += float(x);
				hmInfo.heightMap[index].y += -float(height);
				hmInfo.heightMap[index].z += -float(z);
			} else if (_direction == FACE_DIRECTION::Z_PLUS) {
				hmInfo.heightMap[index].x += -float(x);
				hmInfo.heightMap[index].y += float(z);
				hmInfo.heightMap[index].z += float(height);
			} else if (_direction == FACE_DIRECTION::Z_MINUS) {
				hmInfo.heightMap[index].x += float(x);
				hmInfo.heightMap[index].y += float(z);
				hmInfo.heightMap[index].z += -float(height);
			}

			k += 3;
		}
	}

	//�r�b�g�}�b�v�摜�f�[�^�̉��
	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

void TerrainModel::LoadTexture(const std::string& _filename1, const std::string& _filename2)
{
	// �e�N�X�`������
	std::string filepath;
	if (_filename1 == "null") {
		filepath = "Resources/SubTexture/white1x1.png";
	}
	//�e�N�X�`���L��
	else
	{
		filepath = baseDirectory + _filename1;
	}

	texture[TEXTURE::GRAPHIC_TEX_1] = Texture::Create(filepath);

	// �e�N�X�`������
	if (_filename2 == "null") {
		filepath = "Resources/SubTexture/white1x1.png";
	}
	//�e�N�X�`���L��
	else
	{
		filepath = baseDirectory + _filename2;
	}

	texture[TEXTURE::GRAPHIC_TEX_2] = Texture::Create(filepath);
}

void TerrainModel::CreaetModel(const FACE_DIRECTION _direction)
{
	//����
	direction = _direction;

	int windthSize = hmInfo.terrainWidth - 1;
	int heightSize = hmInfo.terrainHeight - 1;

	int surfaceNum = windthSize * heightSize;
	vertNum = surfaceNum * 4;
	indexNum = surfaceNum * 6;

	std::vector<Mesh::VERTEX> vertices;
	vertices.resize(vertNum);
	std::vector<unsigned long> indices;
	indices.resize(indexNum);

	unsigned long basicsIndices[6] = { 1 ,2 ,0 ,1 ,3 ,2 };

	//�}���C���f�b�N�X�ԍ�
	unsigned long index = 0;

	//�`��p���_���ۑ�
	{
		//���_�ۑ�
		for (int j = 0; j < heightSize; ++j)
		{
			for (int i = 0; i < windthSize; ++i)
			{
				int index1 = (hmInfo.terrainHeight * j) + i;// ����
				int index2 = (hmInfo.terrainHeight * j) + (i + 1);// �E��
				int index3 = (hmInfo.terrainHeight * (j + 1)) + i;// ����
				int index4 = (hmInfo.terrainHeight * (j + 1)) + (i + 1);// �E��

				int vertNum1 = index;
				index++;
				int vertNum2 = index;
				index++;
				int vertNum3 = index;
				index++;
				int vertNum4 = index;
				index++;

				// ����
				vertices[vertNum2].pos = hmInfo.heightMap[index3];
				vertices[vertNum2].uv = XMFLOAT2(1.0f, 0.0f);

				// �E��
				vertices[vertNum4].pos = hmInfo.heightMap[index4];
				vertices[vertNum4].uv = XMFLOAT2(0.0f, 1.0f);

				// ����
				vertices[vertNum1].pos = hmInfo.heightMap[index1];
				vertices[vertNum1].uv = XMFLOAT2(0.0f, 0.0f);

				// �E��
				vertices[vertNum3].pos = hmInfo.heightMap[index2];
				vertices[vertNum3].uv = XMFLOAT2(1.0f, 1.0f);
			}
		}

		//�C���f�b�N�X�ۑ�
		for (int i = 0; i < surfaceNum; i++)
		{
			int vertexNum = i * 4;
			index = i * 6;
			indices[index] = basicsIndices[0] + vertexNum;
			index++;
			indices[index] = basicsIndices[1] + vertexNum;
			index++;
			indices[index] = basicsIndices[2] + vertexNum;
			index++;
			indices[index] = basicsIndices[3] + vertexNum;
			index++;
			indices[index] = basicsIndices[4] + vertexNum;
			index++;
			indices[index] = basicsIndices[5] + vertexNum;
		}

		int normalNum = static_cast<int>(indices.size() / 3);
		for (int i = 0; i < normalNum; i++)
		{
			index = i * 3;
			unsigned long index1 = indices[index];
			index++;
			unsigned long index2 = indices[index];
			index++;
			unsigned long index3 = indices[index];

			XMVECTOR p0 = XMLoadFloat3(&vertices[index1].pos);
			XMVECTOR p1 = XMLoadFloat3(&vertices[index2].pos);
			XMVECTOR p2 = XMLoadFloat3(&vertices[index3].pos);

			XMVECTOR v1 = XMVectorSubtract(p0, p1);
			XMVECTOR v2 = XMVectorSubtract(p0, p2);

			XMVECTOR normal = XMVector3Cross(v1, v2);
			normal = XMVector3Normalize(normal);

			XMStoreFloat3(&vertices[index1].normal, normal);
			XMStoreFloat3(&vertices[index2].normal, normal);
			XMStoreFloat3(&vertices[index3].normal, normal);
		}
	}

	//�����蔻��p���_���ۑ�
	{
		int hitSurfaceNum = windthSize / 2 * heightSize / 2;
		int hitVertNum = hitSurfaceNum * 4;
		int hitIndexNum = hitSurfaceNum * 6;

		hitVertices.resize(hitVertNum);
		hitIndices.resize(hitIndexNum);
		index = 0;
		//���_�ۑ�
		for (int j = 0; j < heightSize - 1; ++j)
		{
			if (j % 2 == 1) { continue; }
			for (int i = 0; i < windthSize - 1; ++i)
			{
				if (i % 2 == 1) { continue; }
				int index1 = (hmInfo.terrainHeight * j) + i;// ����
				int index2 = (hmInfo.terrainHeight * j) + (i + 2);// �E��
				int index3 = (hmInfo.terrainHeight * (j + 2)) + i;// ����
				int index4 = (hmInfo.terrainHeight * (j + 2)) + (i + 2);// �E��

				int vertNum1 = index;
				index++;
				int vertNum2 = index;
				index++;
				int vertNum3 = index;
				index++;
				int vertNum4 = index;
				index++;

				// ����
				hitVertices[vertNum2].pos = hmInfo.heightMap[index3];
				hitVertices[vertNum2].uv = XMFLOAT2(1.0f, 0.0f);

				// �E��
				hitVertices[vertNum4].pos = hmInfo.heightMap[index4];
				hitVertices[vertNum4].uv = XMFLOAT2(0.0f, 1.0f);

				// ����
				hitVertices[vertNum1].pos = hmInfo.heightMap[index1];
				hitVertices[vertNum1].uv = XMFLOAT2(0.0f, 0.0f);

				// �E��
				hitVertices[vertNum3].pos = hmInfo.heightMap[index2];
				hitVertices[vertNum3].uv = XMFLOAT2(1.0f, 1.0f);
			}
		}

		//�C���f�b�N�X�ۑ�
		for (int i = 0; i < hitSurfaceNum; i++)
		{
			int vertexNum = i * 4;
			index = i * 6;
			hitIndices[index] = basicsIndices[0] + vertexNum;
			index++;
			hitIndices[index] = basicsIndices[1] + vertexNum;
			index++;
			hitIndices[index] = basicsIndices[2] + vertexNum;
			index++;
			hitIndices[index] = basicsIndices[3] + vertexNum;
			index++;
			hitIndices[index] = basicsIndices[4] + vertexNum;
			index++;
			hitIndices[index] = basicsIndices[5] + vertexNum;
		}

		//�@���ǉ�
		int normalNum = static_cast<int>(hitIndices.size() / 3);
		for (int i = 0; i < normalNum; i++)
		{
			index = i * 3;
			unsigned long index1 = hitIndices[index];
			index++;
			unsigned long index2 = hitIndices[index];
			index++;
			unsigned long index3 = hitIndices[index];

			XMVECTOR p0 = XMLoadFloat3(&hitVertices[index1].pos);
			XMVECTOR p1 = XMLoadFloat3(&hitVertices[index2].pos);
			XMVECTOR p2 = XMLoadFloat3(&hitVertices[index3].pos);

			XMVECTOR v1 = XMVectorSubtract(p0, p1);
			XMVECTOR v2 = XMVectorSubtract(p0, p2);

			XMVECTOR normal = XMVector3Cross(v1, v2);
			normal = XMVector3Normalize(normal);

			XMStoreFloat3(&hitVertices[index1].normal, normal);
			XMStoreFloat3(&hitVertices[index2].normal, normal);
			XMStoreFloat3(&hitVertices[index3].normal, normal);
		}
	}

	Mesh* mesh = new Mesh;

	//���b�V���֕ۑ�
	for (int i = 0; i < vertNum; i++)
	{
		mesh->AddVertex(vertices[i]);
	}

	//���b�V���֕ۑ�
	for (int i = 0; i < indexNum; i++)
	{
		mesh->AddIndex(indices[i]);
	}

	mesh->CreateBuffers();

	model = new Model;
	model->SetMeshes(mesh);
}
