#include "HeightMap.h"
#include "Camera.h"
#include <DirectXTex.h>
#include <string>
#include "SafeDelete.h"

using namespace Microsoft::WRL;
using namespace DirectX;

ID3D12Device* HeightMap::device = nullptr;
ID3D12GraphicsCommandList* HeightMap::cmdList = nullptr;
std::unique_ptr<GraphicsPipelineManager> HeightMap::pipeline = nullptr;
Camera* HeightMap::camera = nullptr;
LightGroup* HeightMap::lightGroup = nullptr;
const std::string HeightMap::baseDirectory = "Resources/HeightMap/";

void HeightMap::CreateGraphicsPipeline()
{
	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xyz���W
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // �@���x�N�g��
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv���W
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	pipeline = GraphicsPipelineManager::Create("HeightMap",
		GraphicsPipelineManager::OBJECT_KINDS::HEIGHT_MAP, inputLayout, _countof(inputLayout));
}

void HeightMap::StaticInitialize(ID3D12Device* device)
{
	// �ď������`�F�b�N
	assert(!HeightMap::device);
	assert(device);
	HeightMap::device = device;

	CreateGraphicsPipeline();
}

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

void HeightMap::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
	assert(HeightMap::cmdList == nullptr);

	HeightMap::cmdList = cmdList;

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipeline->pipelineState.Get());

	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(pipeline->rootSignature.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void HeightMap::PostDraw()
{
	// �R�}���h���X�g������
	HeightMap::cmdList = nullptr;
}

void HeightMap::Finalize()
{
	pipeline.reset();
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
	HRESULT result = S_FALSE;

	int windthSize = hmInfo.terrainWidth - 1;
	int heightSize = hmInfo.terrainHeight - 1;

	int surfaceNum = windthSize * heightSize;
	vertNum = surfaceNum * 4;
	indexNum = surfaceNum * 6;

	std::vector<Vertex> vertices;
	vertices.resize(vertNum);
	if (vertices.size() == 0)
	{
		assert(0);
	}
	std::vector<unsigned long> indices;
	indices.resize(indexNum);

	unsigned long basicsIndices[6] = { 0 ,2 ,1 ,2 ,3 ,1 };

	//�}���C���f�b�N�X�ԍ�
	unsigned long index = 0;

	//���_�ۑ�
	for (int j = 0; j < heightSize; ++j)
	{
		for (int i = 0; i < windthSize; ++i)
		{
			int index1 = (hmInfo.terrainHeight * j) + i;// Bottom left.
			int index2 = (hmInfo.terrainHeight * j) + (i + 1);// Bottom right.
			int index3 = (hmInfo.terrainHeight * (j + 1)) + i;// Upper left.
			int index4 = (hmInfo.terrainHeight * (j + 1)) + (i + 1);// Upper right.

			// Bottom left.
			vertices[index].pos = hmInfo.heightMap[index1];
			vertices[index].uv = XMFLOAT2(0.0f, 1.0f);
			index++;

			// Upper left.
			vertices[index].pos = hmInfo.heightMap[index3];
			vertices[index].uv = XMFLOAT2(0.0f, 0.0f);
			index++;

			// Bottom right.
			vertices[index].pos = hmInfo.heightMap[index2];
			vertices[index].uv = XMFLOAT2(1.0f, 1.0f);
			index++;

			// Upper right.
			vertices[index].pos = hmInfo.heightMap[index4];
			vertices[index].uv = XMFLOAT2(1.0f, 0.0f);
			index++;
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

		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		XMVECTOR normal = XMVector3Cross(v1, v2);
		normal = XMVector3Normalize(normal);

		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
		XMStoreFloat3(&vertices[index3].normal, normal);
	}

	//���_�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(Vertex) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(vertices.begin(), vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̐���
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(Vertex) * vertNum;
	vbView.StrideInBytes = sizeof(Vertex);

	//�C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(unsigned long) * indexNum), // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	assert(SUCCEEDED(result));

	//�C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned long* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	std::copy(indices.begin(), indices.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R32_UINT;
	ibView.SizeInBytes = sizeof(unsigned long) * indexNum;

	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));
}

HeightMap::~HeightMap()
{
	vertBuff.Reset();
	indexBuff.Reset();
	constBuff.Reset();
	for (int i = 0; i < TEXTURE::Size; i++)
	{
		texture[i].reset();
	}
}

void HeightMap::Update()
{
	//���[���h�s��ϊ�
	XMMATRIX matWorld = XMMatrixIdentity();
	XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matWorld *= matScale;

	XMMATRIX matRot;//�p�x
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(0));
	matRot *= XMMatrixRotationX(XMConvertToRadians(0));
	matRot *= XMMatrixRotationY(XMConvertToRadians(0));
	matWorld *= matRot;

	XMMATRIX matTrans;//���s����
	matTrans = XMMatrixTranslation(0, 0, 0);
	matWorld *= matTrans;

	const XMMATRIX& matViewProjection = camera->GetView() * camera->GetProjection();
	const XMFLOAT3& cameraPos = camera->GetEye();

	//�萔�o�b�t�@�Ƀf�[�^��]��
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);//�}�b�s���O
	assert(SUCCEEDED(result));
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constBuff->Unmap(0, nullptr);
}

void HeightMap::Draw(UINT topology)
{
	//�v���~�e�B�u�`��̐ݒ�R�}���h
	if (topology == 0)
	{
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else if (topology == 1)
	{
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}
	//�萔�o�b�t�@���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//���_�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView);

	//���_�o�b�t�@���Z�b�g
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	// ���C�g�̕`��
	lightGroup->Draw(cmdList, 4);

	//�e�N�X�`���]��
	cmdList->SetGraphicsRootDescriptorTable(1, texture[HeightMapTex]->descriptor->gpu);
	cmdList->SetGraphicsRootDescriptorTable(2, texture[GraphicTex1]->descriptor->gpu);
	cmdList->SetGraphicsRootDescriptorTable(3, texture[GraphicTex2]->descriptor->gpu);

	//�`��R�}���h
	cmdList->DrawIndexedInstanced(indexNum, 1, 0, 0, 0);
}