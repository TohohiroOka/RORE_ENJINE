#include "HeightMap.h"
#include "Camera.h"
#include <DirectXTex.h>
#include <string>
#include "SafeDelete.h"

using namespace Microsoft::WRL;
using namespace DirectX;

GraphicsPipelineManager::GRAPHICS_PIPELINE HeightMap::pipeline;
const std::string HeightMap::baseDirectory = "Resources/HeightMap/";

std::unique_ptr<HeightMap> HeightMap::Create(const std::string heightmapFilename,
	const std::string filename1, const std::string filename2)
{
	//�C���X�^���X�𐶐�
	HeightMap* instance = new HeightMap();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->HeightMapLoad(heightmapFilename);

	instance->LoadTexture(filename1, filename2);

	//������
	instance->Initialize();

	return std::unique_ptr<HeightMap>(instance);
}

void HeightMap::PreDraw()
{
	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipeline.pipelineState.Get());

	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(pipeline.rootSignature.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool HeightMap::HeightMapLoad(const std::string filename)
{
	//���O����
	std::string fname = baseDirectory + filename;

	texture[HeightMapTex] = Texture::Create(fname);

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

	// �摜�f�[�^�̓ǂݍ���
	for (int j = 0; j < hmInfo.terrainHeight; j++)
	{
		for (int i = 0; i < hmInfo.terrainWidth; i++)
		{
			int height = static_cast<int>(bitmapImage[k]);

			int index = (hmInfo.terrainHeight * j) + i;

			hmInfo.heightMap[index].x = (float)i;
			hmInfo.heightMap[index].y = (float)height / 5.0f;
			hmInfo.heightMap[index].z = (float)j;

			k += 3;
		}
	}

	//�r�b�g�}�b�v�摜�f�[�^�̉��
	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

void HeightMap::LoadTexture(const std::string filename1, const std::string filename2)
{
	// �e�N�X�`������
	std::string filepath;
	if (filename1 == "null") {
		filepath = "Resources/SubTexture/white1x1.png";
	}
	//�e�N�X�`���L��
	else
	{
		filepath = baseDirectory + filename1;
	}

	texture[GraphicTex1] = Texture::Create(filepath);

	// �e�N�X�`������
	if (filename2 == "null") {
		filepath = "Resources/SubTexture/white1x1.png";
	}
	//�e�N�X�`���L��
	else
	{
		filepath = baseDirectory + filename2;
	}

	texture[GraphicTex2] = Texture::Create(filepath);
}

void HeightMap::Initialize()
{
	scale = { 25,25,25 };

	HRESULT result = S_FALSE;

	int windthSize = hmInfo.terrainWidth - 1;
	int heightSize = hmInfo.terrainHeight - 1;

	int surfaceNum = windthSize * heightSize;
	vertNum = surfaceNum * 4;
	indexNum = surfaceNum * 6;

	std::vector<Mesh::Vertex> vertices;
	vertices.resize(vertNum);
	std::vector<unsigned long> indices;
	indices.resize(indexNum);

	unsigned long basicsIndices[6] = { 1 ,2 ,0 ,1 ,3 ,2 };

	//�}���C���f�b�N�X�ԍ�
	unsigned long index = 0;

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

	//const int Size = windthSize * 2;

	//for (int i = 0; i < vertNum; i++)
	//{
	//	int index1 = i - Size;// ��
	//	int index2 = i + Size;// ��
	//	int index3 = i - 1;// ��
	//	int index4 = i + 1;// �E

	//	XMFLOAT3 addNormal = { 0,0,0 };

	//	if (index1 > 0)
	//	{
	//		addNormal.x += vertices[index1].normal.x;
	//		addNormal.y += vertices[index1].normal.y;
	//		addNormal.z += vertices[index1].normal.z;
	//	}
	//	if (index2 < vertNum)
	//	{
	//		addNormal.x += vertices[index2].normal.x;
	//		addNormal.y += vertices[index2].normal.y;
	//		addNormal.z += vertices[index2].normal.z;
	//	}

	//	const int retuhaba = ((i / Size) + 1) * Size;
	//	if (index3 > retuhaba -Size)
	//	{
	//		addNormal.x += vertices[index3].normal.x;
	//		addNormal.y += vertices[index3].normal.y;
	//		addNormal.z += vertices[index3].normal.z;
	//	}
	//	if (index4 < retuhaba)
	//	{
	//		addNormal.x += vertices[index4].normal.x;
	//		addNormal.y += vertices[index4].normal.y;
	//		addNormal.z += vertices[index4].normal.z;
	//	}

	//	vertices[i].normal = {
	//		addNormal.x / 4.0f,
	//		addNormal.y / 4.0f,
	//		addNormal.z / 4.0f };
	//}

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

	InterfaceObject3d::Initialize();
}

HeightMap::~HeightMap()
{
	for (int i = 0; i < TEXTURE::Size; i++)
	{
		texture[i].reset();
	}
}

void HeightMap::Draw()
{
	model->VIDraw(cmdList);

	InterfaceObject3d::Draw();

	//�e�N�X�`���]��
	cmdList->SetGraphicsRootDescriptorTable(3, texture[HeightMapTex]->descriptor->gpu);
	cmdList->SetGraphicsRootDescriptorTable(4, texture[GraphicTex1]->descriptor->gpu);
	cmdList->SetGraphicsRootDescriptorTable(5, texture[GraphicTex2]->descriptor->gpu);

	//�`��R�}���h
	cmdList->DrawIndexedInstanced(indexNum, 1, 0, 0, 0);
}