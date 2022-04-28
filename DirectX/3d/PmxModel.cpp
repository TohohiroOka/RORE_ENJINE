#include "PmxModel.h"
#include <DirectXTex.h>
#include <fstream>
#include <array>
#include <string>
#include <vector>

using namespace Microsoft::WRL;
using namespace DirectX;

const std::string PmxModel::baseDirectory = "Resources/PMX/";
ID3D12Device* PmxModel::device = nullptr;

void PmxModel::StaticInitialize(ID3D12Device* device)
{
	// �ď������`�F�b�N
	assert(!PmxModel::device);

	PmxModel::device = device;
}

void PmxModel::LoadTexture()
{
	HRESULT result;

	//�e�N�X�`������
	size_t texSize = texturePaths.size();

	//�e�N�X�`���o�b�t�@�z��̃o�b�t�@�ύX
	texBuffer.resize(texSize);

	for (int i = 0; i < texSize; i++)
	{
		//���O�̃T�C�Y
		const char* pCStrKey = texturePaths[i].c_str();
		int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
		wchar_t* filename = new wchar_t[pSize];
		MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, filename, pSize);

		////WIC�e�N�X�`���̃��[�h
		TexMetadata metadata{};
		ScratchImage scratchImage{};

		result = LoadFromWICFile(
			filename,
			WIC_FLAGS_NONE,
			&metadata, scratchImage);
		assert(SUCCEEDED(result));

		const Image* img = scratchImage.GetImage(0, 0, 0);

		//�e�N�X�`���o�b�t�@�̐���
		ID3D12Resource* addTexBuffer = nullptr;

		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			metadata.width,
			(UINT)metadata.height,
			(UINT16)metadata.arraySize,
			(UINT16)metadata.mipLevels);

		//�e�N�X�`���o�b�t�@����
		result = device->CreateCommittedResource(//GPU���\�[�X�̐���
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,//�e�N�X�`���p�w��
			nullptr,
			IID_PPV_ARGS(&addTexBuffer));
		assert(SUCCEEDED(result));

		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = addTexBuffer->WriteToSubresource(
			0,
			nullptr,//�S�̈�փR�s�[
			img->pixels,//���f�[�^�A�h���X
			(UINT)img->rowPitch,//�P���C���T�C�Y
			(UINT)img->slicePitch//1���T�C�Y
		);
		assert(SUCCEEDED(result));

		//�V�F�[�_�[���\�[�X�r���[�ݒ�
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
		srvDesc.Texture2D.MipLevels = 1;

		//�q�[�v��texnumber�ԖڂɃV�F�[�_�[���\�[�X�r���[���쐬
		device->CreateShaderResourceView(
			addTexBuffer,
			&srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), i,
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

		texBuffer.push_back(addTexBuffer);
	}
}

std::unique_ptr<PmxModel> PmxModel::Create(const std::string& _filePath)
{
	HRESULT result = S_FALSE;

	// �������m��
	PmxModel* instance = new PmxModel();
	
	LoadPMX(_filePath, instance->vertices, instance->surfaces,
		instance->texturePaths, instance->materials, instance->bones);

	instance->Initialize();

	instance->LoadTexture();

	return std::unique_ptr<PmxModel>(instance);
}

void PmxModel::Initialize()
{
	HRESULT result;

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * surfaces.size());

	//���_�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//�C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB), // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	assert(SUCCEEDED(result));

	//���_�o�b�t�@�ւ̃f�[�^�]��
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(vertices.begin(), vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	std::vector<unsigned short> index;
	index.resize(surfaces.size());
	for (int i = 0; i < surfaces.size(); i++)
	{
		index[i] = (unsigned short)surfaces[i].vertexIndex;
	}
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	std::copy(index.begin(), index.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̐���
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1));
	assert(SUCCEEDED(result));

	// �f�X�N���v�^�q�[�v�𐶐�	
	size_t count = materials.size();
	if (count > 0) {
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
		descHeapDesc.NumDescriptors = (UINT)count; // �V�F�[�_�[���\�[�X�r���[�̐�
		result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));//����
		if (FAILED(result)) {
			assert(0);
		}
	}
}

void PmxModel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// �f�X�N���v�^�q�[�v�̔z��
	if (descHeap) {
		ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
		cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	}

	// ���_�o�b�t�@���Z�b�g
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	cmdList->IASetIndexBuffer(&ibView);
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(2,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap->GetGPUDescriptorHandleForHeapStart(),
			0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	// �}�e���A���̒萔�o�b�t�@���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());
	
	// �`��R�}���h
	int firstVertexIndex{};
	for (int i = 0; i < materials.size(); i++)
	{
		if (materials[i].colorMapTextureIndex != PmxModel::NO_DATA_FLAG)
		{
			firstVertexIndex += materials[i].vertexNum;
		}
	}

	//cmdList->DrawIndexed(meshes[_meshIndex].vertexNum, firstVertexIndex, 0);
	cmdList->DrawIndexedInstanced(firstVertexIndex, 1, 0, 0, 0);
}

bool getPMXStringUTF16(std::ifstream& _file, std::string& output)
{
	std::array<wchar_t, 512> wBuffer{};
	int textSize;

	_file.read(reinterpret_cast<char*>(&textSize), 4);

	_file.read(reinterpret_cast<char*>(&wBuffer), textSize);
	output = std::string(&wBuffer[0], &wBuffer[0] + textSize / 2);

	return true;
}

std::vector<PmxModel::Vertex> PmxModel::CollectVertex(std::ifstream& pmxFile, std::array<byte, 8> hederData)
{
	// ���_
	int numberOfVertex{};
	pmxFile.read(reinterpret_cast<char*>(&numberOfVertex), 4);
	std::vector<Vertex> vertices;
	vertices.resize(numberOfVertex);

	for (int i = 0; i < numberOfVertex; i++)
	{
		pmxFile.read(reinterpret_cast<char*>(&vertices[i].position), 12);
		pmxFile.read(reinterpret_cast<char*>(&vertices[i].normal), 12);
		pmxFile.read(reinterpret_cast<char*>(&vertices[i].uv), 8);

		if (hederData[NUMBER_OF_ADD_UV] != 0)
		{
			for (int j = 0; j < hederData[NUMBER_OF_ADD_UV]; ++j)
			{
				pmxFile.read(reinterpret_cast<char*>(&vertices[i].additionalUV[j]), 16);
			}
		}

		const byte weightMethod = pmxFile.get();
		//�{�[���^�C�v1
		if (weightMethod == WeightType::BDEF1)
		{
			vertices[i].weight.type = WeightType::BDEF1;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			vertices[i].weight.born2 = NO_DATA_FLAG;
			vertices[i].weight.born3 = NO_DATA_FLAG;
			vertices[i].weight.born4 = NO_DATA_FLAG;
			vertices[i].weight.weight1 = 1.0f;
		}
		//�{�[���^�C�v2
		else if (weightMethod == WeightType::BDEF2)
		{
			vertices[i].weight.type = WeightType::BDEF2;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
			vertices[i].weight.born3 = NO_DATA_FLAG;
			vertices[i].weight.born4 = NO_DATA_FLAG;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight1), 4);
			vertices[i].weight.weight2 = 1.0f - vertices[i].weight.weight1;
		}
		//�{�[���^�C�v4
		else if (weightMethod == WeightType::BDEF4)
		{
			vertices[i].weight.type = WeightType::BDEF4;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born3), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born4), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight1), 4);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight2), 4);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight3), 4);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight4), 4);
		}
		//�{�[���^�C�v4
		else if (weightMethod == WeightType::SDEF)
		{
			vertices[i].weight.type = WeightType::SDEF;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
			vertices[i].weight.born3 = NO_DATA_FLAG;
			vertices[i].weight.born4 = NO_DATA_FLAG;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight1), 4);
			vertices[i].weight.weight2 = 1.0f - vertices[i].weight.weight1;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.c), 12);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.r0), 12);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.r1), 12);
		}
		//����ȊO��y�ǂݍ��ݎ��s�Ȃ̂ŃG���[���o��
		else
		{
			weightMethod;
			i;
			pmxFile.close();
			assert(0);
		}

		pmxFile.read(reinterpret_cast<char*>(&vertices[i].edgeMagnif), 4);

		//�f�[�^���������߃G���[���o��
		if (vertices[i].weight.born1 == NO_DATA_FLAG)
		{
			pmxFile.close();
			assert(0);
		}
	}

	return vertices;
}

std::vector<PmxModel::Surface> PmxModel::CollectSurface(std::ifstream& pmxFile, std::array<byte, 8> hederData)
{
	int numOfSurface{};
	pmxFile.read(reinterpret_cast<char*>(&numOfSurface), 4);
	std::vector<Surface> surfaces;
	surfaces.resize(numOfSurface);

	for (int i = 0; i < numOfSurface; i++)
	{
		pmxFile.read(reinterpret_cast<char*>(&surfaces[i].vertexIndex), hederData[VERTEX_INDEX_SIZE]);

		if (surfaces[i].vertexIndex == NO_DATA_FLAG ||
			surfaces[i].vertexIndex == NO_DATA_FLAG ||
			surfaces[i].vertexIndex == NO_DATA_FLAG)
		{
			pmxFile.close();
			assert(0);
		}
	}

	return surfaces;
}

std::vector<std::string> PmxModel::CollectTexturePaths(std::ifstream& pmxFile, std::string folderPath)
{
	int numOfTexture{};
	pmxFile.read(reinterpret_cast<char*>(&numOfTexture), 4);

	std::vector<std::string> texturePaths;
	texturePaths.resize(numOfTexture);

	std::string texturePath{};
	for (int i = 0; i < numOfTexture; i++)
	{
		texturePaths[i] = folderPath;
		getPMXStringUTF16(pmxFile, texturePath);
		texturePaths[i] += texturePath;
	}

	return texturePaths;
}

std::vector<PmxModel::Material> PmxModel::CollectMaterial(std::ifstream& pmxFile,
	std::array<byte, 8> hederData, unsigned arrayLength)
{
	int numOfMaterial{};
	pmxFile.read(reinterpret_cast<char*>(&numOfMaterial), 4);

	std::vector<Material>materials;
	materials.resize(numOfMaterial);

	for (int i = 0; i < numOfMaterial; i++)
	{
		for (int j = 0; j < 2; ++j)
		{
			pmxFile.read(reinterpret_cast<char*>(&arrayLength), 4);
			for (unsigned i = 0; i < arrayLength; i++)
			{
				pmxFile.get();
			}
		}

		pmxFile.read(reinterpret_cast<char*>(&materials[i].diffuse), 16);
		pmxFile.read(reinterpret_cast<char*>(&materials[i].specular), 12);
		pmxFile.read(reinterpret_cast<char*>(&materials[i].specularity), 4);
		pmxFile.read(reinterpret_cast<char*>(&materials[i].ambient), 12);

		pmxFile.get();
		for (int i = 0; i < 16; i++)
		{
			pmxFile.get();
		}
		for (int i = 0; i < 4; i++)
		{
			pmxFile.get();
		}

		pmxFile.read(reinterpret_cast<char*>(&materials[i].colorMapTextureIndex), hederData[TEXTURE_INDEX_SIZE]);
		for (unsigned char i = 0; i < hederData[TEXTURE_INDEX_SIZE]; i++)
		{
			pmxFile.get();
		}
		pmxFile.get();

		const byte shareToonFlag = pmxFile.get();
		if (shareToonFlag)
		{
			pmxFile.get();
		} else
		{
			pmxFile.read(reinterpret_cast<char*>(&materials[i].toonTextureIndex), hederData[TEXTURE_INDEX_SIZE]);
		}

		pmxFile.read(reinterpret_cast<char*>(&arrayLength), 4);
		for (unsigned i = 0; i < arrayLength; i++)
		{
			pmxFile.get();
		}

		pmxFile.read(reinterpret_cast<char*>(&materials[i].vertexNum), 4);
	}

	return materials;
}

std::vector<PmxModel::Bone> PmxModel::CollectBone(std::ifstream& pmxFile,
	std::array<byte, 8> hederData, unsigned arrayLength)
{
	int numOfBone{};
	pmxFile.read(reinterpret_cast<char*>(&numOfBone), 4);

	std::vector<Bone> bones;
	bones.resize(numOfBone);

	int ikLinkSize = 0;
	unsigned char angleLim = 0;

	for (int i = 0; i < numOfBone; i++)
	{
		getPMXStringUTF16(pmxFile, bones[i].name);
		pmxFile.read(reinterpret_cast<char*>(&arrayLength), 4);
		bones[i].nameEnglish.resize(arrayLength);
		for (unsigned j = 0; j < arrayLength; ++j)
		{
			bones[i].nameEnglish[j] = pmxFile.get();
		}

		pmxFile.read(reinterpret_cast<char*>(&bones[i].position), 12);

		pmxFile.read(reinterpret_cast<char*>(&bones[i].parentIndex), hederData[BONE_INDEX_SIZE]);
		if (numOfBone <= bones[i].parentIndex)
		{
			bones[i].parentIndex = NO_DATA_FLAG;
		}

		pmxFile.read(reinterpret_cast<char*>(&bones[i].transformationLevel), 4);

		pmxFile.read(reinterpret_cast<char*>(&bones[i].flag), 2);

		enum BoneFlagMask
		{
			ACCESS_POINT = 0x0001,
			IK = 0x0020,
			IMPART_TRANSLATION = 0x0100,
			IMPART_ROTATION = 0x0200,
			AXIS_FIXING = 0x0400,
			LOCAL_AXIS = 0x0800,
			EXTERNAL_PARENT_TRANS = 0x2000,
		};

		if (bones[i].flag & ACCESS_POINT)
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].childrenIndex), hederData[BONE_INDEX_SIZE]);
			if (numOfBone <= bones[i].childrenIndex)
			{
				bones[i].childrenIndex = NO_DATA_FLAG;
			}
		} else
		{
			bones[i].childrenIndex = NO_DATA_FLAG;
			pmxFile.read(reinterpret_cast<char*>(&bones[i].coordOffset), 12);
		}
		if ((bones[i].flag & IMPART_TRANSLATION) || (bones[i].flag & IMPART_ROTATION))
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].impartParentIndex), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&bones[i].impartRate), 4);
		}
		if (bones[i].flag & AXIS_FIXING)
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].fixedAxis), 12);
		}
		if (bones[i].flag & LOCAL_AXIS)
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].localAxisX), 12);
			pmxFile.read(reinterpret_cast<char*>(&bones[i].localAxisZ), 12);
		}
		if (bones[i].flag & EXTERNAL_PARENT_TRANS)
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].externalParentKey), 4);
		}
		if (bones[i].flag & IK)
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].ikTargetIndex), hederData[5]);
			pmxFile.read(reinterpret_cast<char*>(&bones[i].ikLoopCount), 4);
			pmxFile.read(reinterpret_cast<char*>(&bones[i].ikUnitAngle), 4);
			pmxFile.read(reinterpret_cast<char*>(&ikLinkSize), 4);
			bones[i].ikLinks.resize(ikLinkSize);
			for (int j = 0; j < ikLinkSize; ++j)
			{
				pmxFile.read(reinterpret_cast<char*>(&bones[i].ikLinks[j].index), hederData[5]);
				angleLim = pmxFile.get();
				bones[i].ikLinks[j].existAngleLimited = false;
				if (angleLim == 1)
				{
					pmxFile.read(reinterpret_cast<char*>(&bones[i].ikLinks[j].limitAngleMin), 12);
					pmxFile.read(reinterpret_cast<char*>(&bones[i].ikLinks[j].limitAngleMax), 12);
					bones[i].ikLinks[j].existAngleLimited = true;
				}
			}
		} else
		{
			bones[i].ikTargetIndex = NO_DATA_FLAG;
		}
	}

	return bones;
}

void PmxModel::LoadPMX(const std::string& _filePath, std::vector<Vertex>& vertices,
	std::vector<Surface>& surfaces, std::vector<std::string>& texturePaths,
	std::vector<Material>& materials, std::vector<Bone>& bones)
{
	//�p�X����Ȃ�Ԃ�
	if (_filePath.empty())
	{
		assert(0);
	}

	const std::string filename = _filePath + ".pmx";
	const std::string directoryPath = baseDirectory + _filePath + "/";

	// ���f���t�@�C���̃p�X���烂�f���t�H���_�̃p�X�𒊏o���ăI�[�v��
	std::ifstream pmxFile{ directoryPath + filename, (std::ios::binary | std::ios::in) };
	if (pmxFile.fail())
	{
		pmxFile.close();
		assert(0);
	}

	// �w�b�_�[
	std::array<byte, 4> pmxHeader{};
	constexpr std::array<byte, 4> PMX_MAGIC_NUMBER{ 0x50, 0x4d, 0x58, 0x20 };

	for (int i = 0; i < 4; i++)
	{
		pmxHeader[i] = pmxFile.get();
	}
	if (pmxHeader != PMX_MAGIC_NUMBER)
	{
		pmxFile.close();
		assert(0);
	}

	// ver2.0�ȊO�͔�Ή�
	float version{};
	pmxFile.read(reinterpret_cast<char*>(&version), 4);
	if (!DirectX::XMScalarNearEqual(version, 2.0f, DirectX::g_XMEpsilon.f[0]))
	{
		pmxFile.close();
		assert(0);
	}

	byte hederDataLength = pmxFile.get();
	if (hederDataLength != 8)
	{
		pmxFile.close();
		assert(0);
	}
	std::array<byte, 8> hederData{};
	for (int i = 0; i < hederDataLength; i++)
	{
		hederData[i] = pmxFile.get();
	}
	//UTF-8�͔�Ή�
	if (hederData[0] != 0)
	{
		pmxFile.close();
		assert(0);
	}

	//�z��
	unsigned arrayLength{};
	for (int i = 0; i < 4; i++)
	{
		pmxFile.read(reinterpret_cast<char*>(&arrayLength), 4);
		for (unsigned j = 0; j < arrayLength; j++)
		{
			pmxFile.get();
		}
	}

	//���_�̓ǂݍ���
	vertices = CollectVertex(pmxFile, hederData);

	//�ʂ̓ǂݍ���
	surfaces = CollectSurface(pmxFile, hederData);

	//�e�N�X�`���̓ǂݍ���
	texturePaths = CollectTexturePaths(pmxFile, directoryPath);

	//�}�e���A���̓ǂݍ���
	materials = CollectMaterial(pmxFile, hederData, arrayLength);

	//�{�[���̓ǂݍ���
	bones = CollectBone(pmxFile, hederData, arrayLength);

	pmxFile.close();
}