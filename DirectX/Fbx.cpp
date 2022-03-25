#include "Fbx.h"
#include "Camera.h"
#include "BaseCollider.h"
#include <d3dcompiler.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

using namespace DirectX;

FbxManager* Fbx::fbxManager = nullptr;//Fbx�̊��
ID3D12Device* Fbx::device = nullptr;//�f�o�C�X
ID3D12GraphicsCommandList* Fbx::cmdList = nullptr;//�R�}���h���X�g
std::vector<Fbx::Data> Fbx::data;//Fbx�f�[�^�̊i�[�ꏊ
ComPtr<ID3D12PipelineState> Fbx::pipelineState;//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
ComPtr<ID3D12RootSignature> Fbx::rootSignature;//���[�g�V�O�l�`��
ComPtr<ID3D12DescriptorHeap> Fbx::descHeap;//�e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
ComPtr<ID3D12Resource> Fbx::texBuffer[textureNum];//�e�N�X�`�����\�[�X(�e�N�X�`���o�b�t�@)�̔z��
FbxTime Fbx::frameTime;
int Fbx::vecSize = 0;//���݂̔z�񐔊m�F
const std::string Fbx::subTexture = "Resources/subTexture/white1x1.png";
const string Fbx::directoryPath = "Resources/Fbx/";
std::string Fbx::fileName;//�t�@�C���l�[���̕ێ�

Fbx::~Fbx()
{
	int size = data.size();
	for (int i = 0; i < size; i++)
	{
		data[i].fbxUpdate.fbxScene->Destroy();
	}

	data.clear();

	pipelineState.Reset();
	rootSignature.Reset();
	descHeap.Reset();
	for (int i = 0; i < textureNum; i++)
	{
		texBuffer[i].Reset();
	}
}

void Fbx::Pipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	assert(device);

	//�f�X�N���v�^�q�[�v�̐���
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = textureNum;
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/FbxVS.hlsl",    // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/FbxPS.hlsl",    // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy���W(1�s�ŏ������ق������₷��)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv���W(1�s�ŏ������ق������₷��)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;    // �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[3];
	// CBV�i���W�ϊ��s��p�j
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	// SRV�i�e�N�X�`���j
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	// CBV (�X�L�j���O�p)
	rootparams[2].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ���[�g�V�O�l�`���̐���
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = rootSignature.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }
	
	rootSignature->SetName(L"Fbxroot");
	pipelineState->SetName(L"Fbxpipe");
}

void Fbx::LoadMaterial(int dataNumber, FbxNode* fbxNode) {
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
				data[dataNumber].material.ambient.x = (float)ambient.Get()[0];
				data[dataNumber].material.ambient.y = (float)ambient.Get()[1];
				data[dataNumber].material.ambient.z = (float)ambient.Get()[2];

				//�����W��
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				data[dataNumber].material.diffuse.x = (float)diffuse.Get()[0];
				data[dataNumber].material.diffuse.y = (float)diffuse.Get()[1];
				data[dataNumber].material.diffuse.z = (float)diffuse.Get()[2];
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
					string path_str(filePath);
					string name = ExtractFileName(path_str);

					//�e�N�X�`���ǂݍ���
					LoadTexture(dataNumber, directoryPath +fileName + '/' + name);
					textureLoaded = true;
				}
			}
		}

		//texture�������ꍇ���ɂ���
		if (!textureLoaded)
		{
			LoadTexture(dataNumber, subTexture);
		}
	}
}

void Fbx::LoadTexture(int dataNumber, const std::string fname)
{
	HRESULT result;

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
		IID_PPV_ARGS(&texBuffer[dataNumber]));

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texBuffer[dataNumber]->WriteToSubresource(
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
		IID_PPV_ARGS(&texBuffer[dataNumber]));

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texBuffer[dataNumber]->WriteToSubresource(
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
		texBuffer[dataNumber].Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), dataNumber,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
}

void Fbx::CollectMesh(int dataNumber, FbxNode* fbxNode)
{
	//���b�V���擾
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	//���_���ǂݎ��
	CollectVertices(dataNumber, fbxMesh);

	//�ʂ��\������f�[�^�̓ǂݎ��
	CollectMeshFaces(dataNumber, fbxMesh);

	//�}�e���A���̓ǂݎ��
	LoadMaterial(dataNumber, fbxNode);

	//�X�L�j���O���̓ǂݎ��
	CollectSkin(dataNumber, fbxMesh);
}

void Fbx::CollectVertices(int dataNumber, FbxMesh* fbxMesh)
{
	//���_���W�f�[�^��
	const int controlPointCount = fbxMesh->GetControlPointsCount();

	//Fbx���b�V�̒��_���W�擾
	FbxVector4* vertex = fbxMesh->GetControlPoints();

	//�z��̃T�C�Y�ύX
	data[dataNumber].vertices.resize(controlPointCount);

	//���_���R�s�[
	for (int i = 0; i < controlPointCount; i++)
	{
		data[dataNumber].vertices[i].pos.x = (float)vertex[i][0];
		data[dataNumber].vertices[i].pos.y = (float)vertex[i][1];
		data[dataNumber].vertices[i].pos.z = (float)vertex[i][2];
	}
}

void Fbx::CollectMeshFaces(int dataNumber, FbxMesh* fbxMesh)
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
				data[dataNumber].vertices[index].normal.x = (float)normal[0];
				data[dataNumber].vertices[index].normal.y = (float)normal[1];
				data[dataNumber].vertices[index].normal.z = (float)normal[2];
			}

			//�e�N�X�`��uv�ǂݍ���
			if (textureUVCount > 0)
			{
				FbxVector2 uvs;
				bool LUnmappedUV;

				if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, LUnmappedUV))
				{
					data[dataNumber].vertices[index].uv.x = (float)uvs[0];
					data[dataNumber].vertices[index].uv.y = (float)uvs[1];
				}
			}

			//3���_�܂ł̃C���f�b�N�X�ǉ�
			if (j < 3)
			{
				data[dataNumber].indices.push_back(index);
			}
			//4���_�ڂ�����ꍇ
			else
			{
				//���i�[
				int index2 = data[dataNumber].indices[data[dataNumber].indices.size() - 1];
				int index3 = index;
				int index0 = data[dataNumber].indices[data[dataNumber].indices.size() - 3];

				//�{�i�[
				data[dataNumber].indices.push_back(index2);
				data[dataNumber].indices.push_back(index3);
				data[dataNumber].indices.push_back(index0);
			}
		}
	}
}

void Fbx::CollectSkin(int dataNumber, FbxMesh* fbxMesh)
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
	vector<Bone>& bones = data[dataNumber].bones;

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
	vector<list<WeightSet>> weightLists(data[dataNumber].vertices.size());

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
	auto& vertices = data[dataNumber].vertices;

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

void Fbx::LoadNode(int dataNumber, FbxNode* fbxNode, Node* parent)
{
	Node virtualNode;

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
			CollectMesh(dataNumber, fbxNode);
		}
	}

	//���ϐ��ɓ��ꂽ���̂��R���e�i�̈�Ԍ��ɃZ�b�g
	data[dataNumber].nodes.push_back(virtualNode);

	//�q�m�[�h������΍ċA����
	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		LoadNode(dataNumber, fbxNode->GetChild(i), &virtualNode);
	}
}

void Fbx::LoadAnimation(int dataNumber)
{
	//0�Ԗڂ̃A�j���[�V�����擾
	FbxAnimStack* animstick = data[dataNumber].fbxUpdate.fbxScene->GetSrcObject<FbxAnimStack>(0);
	//�A�j���[�V�����̖��O�擾
	const char* anumstackkname = animstick->GetName();
	//�A�j���[�V�����̎��ԏ��
	FbxTakeInfo* takeinfo = data[dataNumber].fbxUpdate.fbxScene->GetTakeInfo(anumstackkname);

	//�J�n����
	data[dataNumber].fbxUpdate.startTime = takeinfo->mLocalTimeSpan.GetStart();

	//�I������
	data[dataNumber].fbxUpdate.stopTime = takeinfo->mLocalTimeSpan.GetStop();

	//�J�n���Ԃɍ��킹��
	data[dataNumber].fbxUpdate.nowTime = data[dataNumber].fbxUpdate.startTime;

	//�Đ��\
	data[dataNumber].fbxUpdate.isAnimation = true;
}

int Fbx::LoadFbx(const std::string fileName)
{	
	//���݂̔z��ԍ�
	const int dataNumber = data.size();

	//��̃f�[�^������₷
	Data addData;
	data.push_back(addData);

	//���O�̓o�^
	Fbx::fileName = fileName;

	//�t�@�C���̌^
	const string shape = ".fbx";

	//string�^�Ō���
	const string strFileName = directoryPath + fileName + '/' + fileName + shape;

	//const char*�ɕϊ�
	const char* cFileName = strFileName.c_str();

	//�t�@�C���ǂݍ���
	data[dataNumber].fbxUpdate.fbxScene = FbxScene::Create(fbxManager, "fbxscene");
	FbxString FileName(cFileName);
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "imp");
	fbxImporter->Initialize(FileName.Buffer(), -1, fbxManager->GetIOSettings());
	fbxImporter->Import(data[dataNumber].fbxUpdate.fbxScene);
	fbxImporter->Destroy();

	//�m�[�h�ǂݍ���
	LoadNode(dataNumber, data[dataNumber].fbxUpdate.fbxScene->GetRootNode());
	
	LoadAnimation(dataNumber);

	return dataNumber;
}

void Fbx::StaticInitialize(ID3D12Device* device)
{
	Fbx::device = device;

	Pipeline();
	pipelineState->SetName(L"fbxpi");
	rootSignature->SetName(L"fbxro");

	fbxManager = FbxManager::Create();

	//�t���[���ݒ�
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
}

void Fbx::Create() {
	HRESULT result = S_FALSE;

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * data[modelNumber].vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * data[modelNumber].indices.size());

	//���_�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));


	//�C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB), // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//���_�o�b�t�@�ւ̃f�[�^�]��
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(data[modelNumber].vertices.begin(), data[modelNumber].vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	std::copy(data[modelNumber].indices.begin(), data[modelNumber].indices.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̐���
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(data[modelNumber].vertices[0]);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	// �萔�o�b�t�@1�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform));

	//�萔�o�b�t�@Skin�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin));

}

void Fbx::CreateModel(UINT modelNumber) {
	//�e�N�X�`���ԍ��R�s�[
	this->modelNumber = modelNumber;

	//obj����
	Create();
}

void Fbx::Update(Camera* camera)
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	//�A�j���[�V����
	if (isAnimation)
	{
		data[modelNumber].fbxUpdate.nowTime += frameTime;
		//�Ō�܂ōs������擪�ɖ߂�
		if (data[modelNumber].fbxUpdate.nowTime > data[modelNumber].fbxUpdate.stopTime)
		{
			data[modelNumber].fbxUpdate.nowTime = data[modelNumber].fbxUpdate.startTime;
		}
	}

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
	matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

	// �萔�o�b�t�@1�փf�[�^�]��
	ConstBufferDataTransform* constMap = nullptr;
	result = constBuffTransform->Map(0, nullptr, (void**)&constMap);
	constMap->viewproj = camera->GetView() * camera->GetProjection();// �s��̍���
	constMap->world = matWorld;
	constMap->cameraPos = camera->GetEye();
	constMap->color = { 1.0f,1.0f,1.0f,1.0f };
	constBuffTransform->Unmap(0, nullptr);

	//�{�[���z��擾
	vector<Bone>& bones = data[modelNumber].bones;

	// �萔�o�b�t�@Skin�փf�[�^�]��
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < bones.size(); i++)
	{
		//���̎p���s��
		XMMATRIX matCurrentPose;
		//���݂̎p�����擾
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(data[modelNumber].fbxUpdate.nowTime);
		//XMMATRIX�ɕϊ�
		ConvertMatrixFormFbx(&matCurrentPose, fbxCurrentPose);
		//�������ăX�L�j���O�s��ɕۑ�
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}
	constBuffSkin->Unmap(0, nullptr);
}

void Fbx::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	Fbx::cmdList = cmdList;

	//�p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelineState.Get());

	//���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(rootSignature.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�f�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

}

void Fbx::PostDraw()
{
	// �R�}���h���X�g������
	Fbx::cmdList = nullptr;
}

void Fbx::Draw(int modelNumber)
{
	//���ڂ̌Ăяo������Ă΂�Ȃ�
	if (this->modelNumber != modelNumber)
	{
		CreateModel(modelNumber);
	}

	//���_�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView);

	//���_�o�b�t�@���Z�b�g
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//�萔�o�b�t�@���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffTransform->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootConstantBufferView(2, constBuffSkin->GetGPUVirtualAddress());

	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			modelNumber,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//�`��R�}���h
	cmdList->DrawIndexedInstanced((UINT)data[modelNumber].indices.size(), 1, 0, 0, 0);
}

std::string Fbx::ExtractFileName(const std::string& path)
{
	size_t pos1;

	//��؂蕶��'\\'���o�Ă����ԍŌ�̕���������

	pos1 = path.rfind('\\');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	//��؂蕶��'/'���o�Ă����ԍŌ�̕���������
	pos1 = path.rfind('/');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}

void Fbx::ConvertMatrixFormFbx(DirectX::XMMATRIX* dst, const FbxMatrix& src)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			dst->r[i].m128_f32[j] = (float)src.Get(i, j);
		}
	}
}