#include "FbxModel.h"

#include <DirectXTex.h>
#include <string>

using namespace Microsoft::WRL;
using namespace DirectX;

ID3D12Device* FbxModel::device = nullptr;
FbxManager* FbxModel::fbxManager = nullptr;
FbxImporter* FbxModel::fbxImporter = nullptr;
ComPtr<ID3D12DescriptorHeap> FbxModel::descHeap;
ComPtr<ID3D12Resource> FbxModel::texBuffer[textureNum];
FbxTime FbxModel::frameTime;
const std::string FbxModel::defaultTexture = "Resources/subTexture/white1x1.png";
const std::string FbxModel::baseDirectory = "Resources/Fbx/";

FbxModel::~FbxModel()
{
	constBuffSkin.Reset();
	vertBuff.Reset();
	indexBuff.Reset();
}

void FbxModel::StaticInitialize(ID3D12Device* device)
{
	HRESULT result = S_FALSE;

	// �ď������`�F�b�N
	assert(!FbxModel::device);

	// nullptr�`�F�b�N
	assert(device);

	FbxModel::device = device;

	//�f�X�N���v�^�q�[�v�̐���
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = textureNum;
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));

	fbxManager = FbxManager::Create();
	fbxImporter = FbxImporter::Create(fbxManager, "imp");

	//�t���[���ݒ�
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
}

void FbxModel::LoadMaterial(FbxNode* fbxNode)
{
	HRESULT result = S_FALSE;

	//�e�N�X�`���̗L���m�F
	bool textureLoaded = false;

	//�}�e���A���̌��m�F
	const int materialCount = fbxNode->GetMaterialCount();

	if (materialCount > 0)
	{
		//�擪�̃}�e���A�����擾
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);

		if (material)
		{
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);

				//�����W��
				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				data->material.ambient.x = (float)ambient.Get()[0];
				data->material.ambient.y = (float)ambient.Get()[1];
				data->material.ambient.z = (float)ambient.Get()[2];

				//�����W��
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				data->material.diffuse.x = (float)diffuse.Get()[0];
				data->material.diffuse.y = (float)diffuse.Get()[1];
				data->material.diffuse.z = (float)diffuse.Get()[2];
			}

			//�f�B�t���[�Y�e�N�X�`��
			const FbxProperty diffuseProperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (diffuseProperty.IsValid())
			{
				const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();

				if (texture)
				{
					const char* filePath = texture->GetFileName();

					//�t�@�C���p�X����t�@�C�����擾
					std::string path_str(filePath);
					std::string fileName = ExtractFileName(path_str);

					//�e�N�X�`���ǂݍ���
					LoadTexture(baseDirectory + name + '/' + fileName);
					textureLoaded = true;
				}
			}
		}

		//texture�������ꍇ���ɂ���
		if (!textureLoaded)
		{
			LoadTexture(defaultTexture);
		}
	}
}

void FbxModel::LoadTexture(const std::string fname)
{
	HRESULT result;

	//�e�N�X�`���o�b�t�@�ԍ�
	static int texBuffNum = 0;

	////WIC�e�N�X�`���̃��[�h
	TexMetadata metadata{};
	ScratchImage scratchImage{};

	//���j�R�[�h�ɕϊ�
	wchar_t wfilePath[128];
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0,
		fname.c_str(), -1, wfilePath, _countof(wfilePath));

	result = LoadFromWICFile(
		wfilePath,
		WIC_FLAGS_NONE,
		&metadata, scratchImage);
	assert(SUCCEEDED(result));

	const Image* img = scratchImage.GetImage(0, 0, 0);

	//�e�N�X�`���o�b�t�@�̐���
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
		IID_PPV_ARGS(&texBuffer[texBuffNum]));

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texBuffer[texBuffNum]->WriteToSubresource(
		0,
		nullptr,//�S�̈�փR�s�[
		img->pixels,//���f�[�^�A�h���X
		(UINT)img->rowPitch,//�P���C���T�C�Y
		(UINT)img->slicePitch//1���T�C�Y
	);

	//�e�N�X�`���o�b�t�@����
	result = device->CreateCommittedResource(//GPU���\�[�X�̐���
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,//�e�N�X�`���p�w��
		nullptr,
		IID_PPV_ARGS(&texBuffer[texBuffNum]));

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texBuffer[texBuffNum]->WriteToSubresource(
		0,
		nullptr,//�S�̈�փR�s�[
		img->pixels,//���f�[�^�A�h���X
		(UINT)img->rowPitch,//�P���C���T�C�Y
		(UINT)img->slicePitch//1���T�C�Y
	);

	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;

	//�q�[�v��texnumber�ԖڂɃV�F�[�_�[���\�[�X�r���[���쐬
	device->CreateShaderResourceView(
		texBuffer[texBuffNum].Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), 0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	texBuffNum++;
}

void FbxModel::CollectMesh(FbxNode* fbxNode)
{
	//���b�V���擾
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	//���_���ǂݎ��
	CollectVertices(fbxMesh);

	//�ʂ��\������f�[�^�̓ǂݎ��
	CollectMeshFaces(fbxMesh);

	//�}�e���A���̓ǂݎ��
	LoadMaterial(fbxNode);

	//�X�L�j���O���̓ǂݎ��
	CollectSkin(fbxMesh);
}

void FbxModel::CollectVertices(FbxMesh* fbxMesh)
{
	//���_���W�f�[�^��
	const int controlPointCount = fbxMesh->GetControlPointsCount();

	//Fbx���b�V���̒��_���W�擾
	FbxVector4* vertex = fbxMesh->GetControlPoints();

	//�z��̃T�C�Y�ύX
	data->vertices.resize(controlPointCount);

	//���_���R�s�[
	for (int i = 0; i < controlPointCount; i++)
	{
		data->vertices[i].pos.x = (float)vertex[i][0];
		data->vertices[i].pos.y = (float)vertex[i][1];
		data->vertices[i].pos.z = (float)vertex[i][2];
	}
}

void FbxModel::CollectMeshFaces(FbxMesh* fbxMesh)
{
	//�ʂ̐�
	const int polygonCount = fbxMesh->GetPolygonCount();

	//UV�f�[�^�̐�
	const int textureUVCount = fbxMesh->GetTextureUVCount();

	//UV�����X�g
	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);

	//�ʂ��Ƃ̏��
	for (int i = 0; i < polygonCount; i++)
	{
		//�ʂ̒��_���擾
		const int polygonSize = fbxMesh->GetPolygonSize(i);
		assert(polygonSize <= 4);

		//1���_���Ƃɓǂݍ���
		for (int j = 0; j < polygonSize; j++)
		{
			int index = fbxMesh->GetPolygonVertex(i, j);
			assert(index >= 0);

			//�擾�����@�����ꎞ�i�[����
			FbxVector4 normal;

			//�@���ǂݍ���
			if (fbxMesh->GetPolygonVertexNormal(i, j, normal))
			{
				data->vertices[index].normal.x = (float)normal[0];
				data->vertices[index].normal.y = (float)normal[1];
				data->vertices[index].normal.z = (float)normal[2];
			}

			//�e�N�X�`��uv�ǂݍ���
			if (textureUVCount > 0)
			{
				FbxVector2 uvs;
				bool LUnmappedUV;

				if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, LUnmappedUV))
				{
					data->vertices[index].uv.x = (float)uvs[0];
					data->vertices[index].uv.y = (float)uvs[1];
				}
			}

			//3���_�܂ł̃C���f�b�N�X�ǉ�
			if (j < 3)
			{
				data->indices.push_back(index);
			}
			//4���_�ڂ�����ꍇ
			else
			{
				//���i�[
				int index2 = data->indices[data->indices.size() - 1];
				int index3 = index;
				int index0 = data->indices[data->indices.size() - 3];

				//�{�i�[
				data->indices.push_back(index2);
				data->indices.push_back(index3);
				data->indices.push_back(index0);
			}
		}
	}
}

void FbxModel::CollectSkin(FbxMesh* fbxMesh)
{
	//�X�L�j���O���
	FbxSkin* fbxSkin =
		static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));

	//�X�L�j���O��񂪖�����ΏI��
	if (fbxSkin == nullptr)
	{
		return;
	}

	//�{�[���z��̎Q��
	std::vector<Bone>& bones = data->bones;

	//�{�[���̐�
	int clusterCount = fbxSkin->GetClusterCount();

	//�z��m��
	bones.reserve(clusterCount);

	//�S�Ẵ{�[���ɂ���
	for (int i = 0; i < clusterCount; i++)
	{
		//Fbx�{�[�����
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		//�{�[�����̂̃m�[�h�̖��O���擾
		const char* boenName = fbxCluster->GetLink()->GetName();

		//�{�[����ǉ����ĎQ�Ƃ𓾂�
		bones.emplace_back(Bone(boenName));
		Bone& bone = bones.back();

		//����{�[����FBX��R�Â���
		bone.fbxCluster = fbxCluster;

		//FBX���珉���p���s����擾
		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		//XMMatrix�^�ɕϊ�
		XMMATRIX initialPos;
		ConvertMatrixFormFbx(&initialPos, fbxMat);

		//�����p���s��̋t�s��̎擾
		bone.invInitialPose = XMMatrixInverse(nullptr, initialPos);
	}

	//�X�L���E�F�C�g
	struct WeightSet
	{
		UINT index;
		float weight;
	};

	//�ۑ��p�z��
	std::vector<std::list<WeightSet>> weightLists(data->vertices.size());

	//�S�Ẵ{�[���ɂ���
	for (int i = 0; i < clusterCount; i++)
	{
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		//�{�[���ɉe�����󂯂钸�_�̐�
		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();

		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controlWeights = fbxCluster->GetControlPointWeights();

		//�e�����󂯂�S���_�ɂ���
		for (int j = 0; j < controlPointIndicesCount; j++)
		{
			//���_�ԍ�
			int vertIndex = controlPointIndices[j];

			//�X�L���E�F�C�g
			float weight = (float)controlWeights[j];

			//���̒��_�̉e�����󂯂�{�[�����X�g�ɁA�{�[�����E�F�C�g�̃y�A��ǉ�
			weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
		}
	}

	//���_�z�񏑂������p�̎Q��
	auto& vertices = data->vertices;

	//�e���_�ɂ��ď���
	for (int i = 0; i < vertices.size(); i++)
	{
		//���_�̃E�F�C�g����ł��傫��4��I��
		auto& weightList = weightLists[i];

		//�召���p�̃����_�����w�肵�č~���Ƀ\�[�g
		weightList.sort(
			[](auto const& lhs, auto const& rhs)
			{
				//���̗v�f���傫�����true,�łȂ����false
				return lhs.weight > rhs.weight;
			});

		int weightArrayIndex = 0;

		//�~���\�[�g�ς݂̃E�F�C�g���X�g����
		for (auto& WeightSet : weightList)
		{
			//���_�f�[�^�ɏ�������
			vertices[i].boneIndex[weightArrayIndex] = WeightSet.index;
			vertices[i].boneWhight[weightArrayIndex] = WeightSet.weight;

			//4�ɒB������I��
			if (++weightArrayIndex >= MAX_BONE_INDICES)
			{
				float weight = 0.0f;

				//2�Ԗڈȍ~�̃E�F�C�g�v�Z
				for (int j = 1; j < MAX_BONE_INDICES; j++)
				{
					weight += vertices[i].boneWhight[j];
				}

				//���v��1.0f(100%)�ɂȂ�悤����
				vertices[i].boneWhight[0] = 1.0f - weight;
				break;
			}
		}
	}
}

void FbxModel::LoadNode(FbxNode* fbxNode, Node* parent)
{
	data->nodes.emplace_back();
	Node& virtualNode = data->nodes.back();

	virtualNode.name = fbxNode->GetName();

	//�m�[�h�̃��[�J�����
	FbxDouble3 rota = fbxNode->LclRotation;
	FbxDouble3 scaling = fbxNode->LclScaling;
	FbxDouble3 translation = fbxNode->LclTranslation;

	//�`���ϊ����đ��
	virtualNode.rotation = { (float)rota[0], (float)rota[1], (float)rota[2], 0.0f };
	virtualNode.scaling = { (float)scaling[0], (float)scaling[1], (float)scaling[2], 0.0f };
	virtualNode.translation = { (float)translation[0], (float)translation[1], (float)translation[2], 0.0f };

	//�s��v�Z
	XMMATRIX matScaling, matRoration, matTranslation;
	matScaling = XMMatrixScalingFromVector(virtualNode.scaling);
	matRoration = XMMatrixRotationRollPitchYawFromVector(virtualNode.rotation);
	matTranslation = XMMatrixTranslationFromVector(virtualNode.translation);

	//�ό`�s��v�Z
	virtualNode.transform = XMMatrixIdentity();
	virtualNode.transform *= matScaling;
	virtualNode.transform *= matRoration;
	virtualNode.transform *= matTranslation;

	//�O���[�o���s��v�Z
	virtualNode.globalTransform = virtualNode.transform;
	if (parent)
	{
		//�e��o�^
		virtualNode.parent = parent;

		//�e�̕ό`����Z
		virtualNode.globalTransform *= parent->globalTransform;
	}

	//��t�b�V�����̉��
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute)
	{
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			//���ϐ��ɓ��ꂽ���̂��R���e�i�̈�Ԍ��ɃZ�b�g
			data->meshNode = &virtualNode;

			CollectMesh(fbxNode);
		}
	}

	//�q�m�[�h������΍ċA����
	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		LoadNode(fbxNode->GetChild(i), &virtualNode);
	}
}

void FbxModel::LoadAnimation(FbxScene* fbxScene)
{
	//0�Ԗڂ̃A�j���[�V�����擾
	FbxAnimStack* animstick = fbxScene->GetSrcObject<FbxAnimStack>(0);
	//�A�j���[�V�����̖��O�擾
	const char* anumstackkname = animstick->GetName();
	//�A�j���[�V�����̎��ԏ��
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(anumstackkname);

	//�J�n����
	data->fbxUpdate.startTime = takeinfo->mLocalTimeSpan.GetStart();

	//�I������
	data->fbxUpdate.stopTime = takeinfo->mLocalTimeSpan.GetStop();

	//�J�n���Ԃɍ��킹��
	data->fbxUpdate.nowTime = data->fbxUpdate.startTime;

	//�Đ��\
	data->fbxUpdate.isAnimation = true;
}

void FbxModel::LoadFbx(const std::string modelName)
{
	//���O�̓o�^
	name = modelName;
	// ���f���Ɠ������O�̃t�H���_����ǂݍ���
	const std::string directoryPath = baseDirectory + modelName + "/";
	// �g���q.fbx��t��
	const std::string fileName = modelName + ".fbx";
	// �A�����ăt���p�X�𓾂�
	const std::string fullpath = directoryPath + fileName;

	// �t�@�C�������w�肵��FBX�t�@�C����ǂݍ���
	if (!fbxImporter->Initialize(fullpath.c_str(), -1, fbxManager->GetIOSettings())) {
		assert(0);
	}

	// �V�[������
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "fbxScene");
	// �t�@�C�����烍�[�h����FBX�̏����V�[���ɃC���|�[�g
	fbxImporter->Import(fbxScene);

	// FBX�m�[�h�̐����擾
	int nodeCount = fbxScene->GetNodeCount();
	// ���炩���ߕK�v�����̃��������m�ۂ��邱�ƂŁA�A�h���X�������̂�\�h
	data->nodes.reserve(nodeCount);

	//�m�[�h�ǂݍ���
	LoadNode(fbxScene->GetRootNode());

	//�A�j���[�V�����̐ݒ�
	LoadAnimation(fbxScene);

	data->fbxUpdate.fbxScene = fbxScene;
}

void FbxModel::Initialize()
{
	HRESULT result = S_FALSE;

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * data->vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * data->indices.size());

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
	std::copy(data->vertices.begin(), data->vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	std::copy(data->indices.begin(), data->indices.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̐���
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(data->vertices[0]);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	//�萔�o�b�t�@Skin�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@Skin�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin));
	assert(SUCCEEDED(result));
}

std::unique_ptr<FbxModel> FbxModel::Create(const std::string fileName)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	FbxModel* instance = new FbxModel();

	instance->data = std::make_unique<Data>();

	//Fbx�t�@�C���̓ǂݍ���
	instance->LoadFbx(fileName);

	//Fbx�̏����ݒ�
	instance->Initialize();

	return std::unique_ptr<FbxModel>(instance);
}

void FbxModel::Update()
{
	HRESULT result;

	//�A�j���[�V����
	if (isAnimation)
	{
		data->fbxUpdate.nowTime += frameTime;
		//�Ō�܂ōs������擪�ɖ߂�
		if (data->fbxUpdate.nowTime > data->fbxUpdate.stopTime)
		{
			data->fbxUpdate.nowTime = data->fbxUpdate.startTime;
		}
	}

	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataB1* constMap = nullptr;
	result = constBuffB1->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->ambient = data->material.ambient;
		constMap->diffuse = data->material.diffuse;
		constMap->specular = data->material.specular;
		constMap->alpha = data->material.alpha;
		constBuffB1->Unmap(0, nullptr);
	}

	//�{�[���z��擾
	std::vector<Bone>& bones = data->bones;

	// �萔�o�b�t�@Skin�փf�[�^�]��
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < bones.size(); i++)
	{
		//���̎p���s��
		XMMATRIX matCurrentPose;
		//���݂̎p�����擾
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(data->fbxUpdate.nowTime);
		//XMMATRIX�ɕϊ�
		ConvertMatrixFormFbx(&matCurrentPose, fbxCurrentPose);
		//�������ăX�L�j���O�s��ɕۑ�
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}
	constBuffSkin->Unmap(0, nullptr);
}

void FbxModel::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	//�f�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

}

void FbxModel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//���_�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView);

	//���_�o�b�t�@���Z�b�g
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//�萔�o�b�t�@���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootConstantBufferView(4, constBuffSkin->GetGPUVirtualAddress());

	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(2,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//�`��R�}���h
	cmdList->DrawIndexedInstanced((UINT)data->indices.size(), 1, 0, 0, 0);
}

void FbxModel::Finalize()
{
	descHeap.Reset();
	for (int i = 0; i < textureNum; i++)
	{
		texBuffer[i].Reset();
	}

	fbxImporter->Destroy();
	fbxManager->Destroy();
}

std::string FbxModel::ExtractFileName(const std::string& path)
{
	size_t pos1;

	//��؂蕶��'\\'���o�Ă����ԍŌ�̕���������

	pos1 = path.rfind('\\');
	if (pos1 != std::string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	//��؂蕶��'/'���o�Ă����ԍŌ�̕���������
	pos1 = path.rfind('/');
	if (pos1 != std::string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}

void FbxModel::ConvertMatrixFormFbx(DirectX::XMMATRIX* dst, const FbxMatrix& src)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			dst->r[i].m128_f32[j] = (float)src.Get(i, j);
		}
	}
}