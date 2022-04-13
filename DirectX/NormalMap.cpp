#include "NormalMap.h"
#include "Camera.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include"Camera.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

using namespace std;

ID3D12Device* NormalMap::device = nullptr;//�f�o�C�X
ID3D12GraphicsCommandList* NormalMap::cmdList = nullptr;//�R�}���h���X�g
ComPtr<ID3D12PipelineState> NormalMap::pipelineState;//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
ComPtr<ID3D12RootSignature> NormalMap::rootSignature;//���[�g�V�O�l�`��
ComPtr<ID3D12DescriptorHeap> NormalMap::descHeap;//�e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
ComPtr<ID3D12Resource> NormalMap::texBuffer[textureNum];//�e�N�X�`�����\�[�X(�e�N�X�`���o�b�t�@)�̔z��
int NormalMap::alltextureNum = 0;
//���_�f�[�^
NormalMap::Vertex NormalMap::vertices[24] = {
	//�O
	{{ -edge, -edge, -edge },{},{0.0f,1.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����0
	{{ -edge, edge, -edge },{},{0.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����1
	{{ edge, -edge, -edge },{}, { 1.0f,1.0f },{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��2
	{{ edge, edge, -edge },{}, {1.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��3
	//��
	{{ -edge, edge, edge },{}, {0.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����4
	{{ -edge, -edge, edge },{}, {0.0f,1.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����5
	{{ edge, edge, edge },{}, {1.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��6
	{{ edge, -edge, edge },{}, {1.0f,1.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��7
	//��
	{{ -edge, -edge, -edge },{}, {0.0f,1.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����8
	{{ -edge, -edge, edge },{}, {0.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����9
	{{ -edge, edge, -edge },{}, {1.0f,1.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��10
	{{ -edge, edge, edge },{}, {1.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��11
	//�E
	{{ edge, -edge, edge },{}, {0.0f,1.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����12
	{{ edge, -edge, -edge },{}, {0.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����13
	{{ edge, edge, edge },{}, {1.0f,1.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��14
	{{ edge, edge, -edge },{}, {1.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��15
	//��
	{{ -edge, edge, -edge },{}, {0.0f,1.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����16
	{{ -edge, edge, edge },{}, {0.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����17
	{{ edge, edge, -edge },{}, {1.0f,1.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��18
	{{ edge, edge, edge },{}, {1.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��19
	//��
	{{ -edge, -edge, edge },{}, {0.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����20
	{{ -edge, -edge, -edge },{}, {0.0f,1.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // ����21
	{{ edge, -edge, edge },{}, {1.0f,0.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��22
	{{ edge, -edge, -edge },{}, {1.0f,1.0f},{1.0f, 0.0f, 0.0f},{0.0f, -1.0f, 0.0f}}, // �E��23
};
//�C���f�b�N�X�f�[�^
const unsigned short NormalMap::indices[36] = {
	//�O
	0,1,2,2,1,3,
	//��
	4,5,6,6,5,7,
	//��
	8,9,10,10,9,11,
	//�E
	12,13,14,14,13,15,
	//��
	16,17,18,18,17,19,
	//��
	20,21,22,22,21,23,
};


NormalMap::~NormalMap()
{
	vertBuff.Reset();
	indexBuff.Reset();
	constBuff.Reset();
}

void NormalMap::AllDelete()
{
	pipelineState.Reset();
	rootSignature.Reset();
	descHeap.Reset();
	for (int i = 0; i < textureNum; i++)
	{
		texBuffer[i].Reset();
	}
}

void NormalMap::Pipeline()
{
	HRESULT result;
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	//���_�V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/BasicVS.hlsl",  // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
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

	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/BasicPS.hlsl",   // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
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

	////���_���C�A�E�g�z��̐錾�Ɛݒ�
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"NORMAL", 0,DXGI_FORMAT_R32G32B32_FLOAT,
			0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		 },
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
			0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TANGENT", 0,DXGI_FORMAT_R32G32B32_FLOAT,
			0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		 },
		{
			"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	////�p�C�v���C���X�e�[�g�ݒ�
	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//���X�^���C�U�X�e�[�g
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//�u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA�S�Ẵ`�����l����`��

	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//�W���ݒ�
	blenddesc.BlendEnable = true;//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//�e�X�g�̒l��0%�g��
	//����������
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//�\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//�e�X�g�̒l��100%�g��

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1; // �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	//�[�x�X�e���V���X�e�[�g�ݒ�
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;//�[�x�l�t�H�[�}�b�g

	//�f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV[3];
	descRangeSRV[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0���W�X�^
	descRangeSRV[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//t0���W�X�^
	descRangeSRV[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);//t0���W�X�^

	//���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[4];
	rootparams[0].InitAsConstantBufferView(0);//�萔�o�b�t�@�r���[�Ƃ��ď�����
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV[0], D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV[1], D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsDescriptorTable(1, &descRangeSRV[2], D3D12_SHADER_VISIBILITY_ALL);

	//�e�N�X�`���T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//�o�[�W������������ł̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);

	//���[�g�V�O�l�`���̐���
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = rootSignature.Get();

	////�O���t�B�b�N�X�p�C�v���C���X�e�[�g�̐���
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineState));
}

void NormalMap::CommonCreate() {
	HRESULT result = S_FALSE;

	//�f�X�N���v�^�q�[�v�̐���
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = textureNum;
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));
}

int NormalMap::LoadTexture(const wchar_t* filename)
{
	HRESULT result;

	//�ُ�Ȕԍ��̎w��
	assert(alltextureNum <= textureNum - 1);

	////WIC�e�N�X�`���̃��[�h
	TexMetadata metadata{};
	ScratchImage scratchImage{};

	result = LoadFromWICFile(
		filename,
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
		IID_PPV_ARGS(&texBuffer[alltextureNum]));

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texBuffer[alltextureNum]->WriteToSubresource(
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
		texBuffer[alltextureNum].Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), alltextureNum,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//�e�N�X�`����
	alltextureNum++;

	return alltextureNum - 1;
}

void NormalMap::Create() {
	HRESULT result = S_FALSE;

	//���_�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	//�@�������̌v�Z
	for (int i = 0; i < _countof(indices) / 3; i++)
	{
		//�ꎞ�I�Ɋi�[
		unsigned short index_0 = indices[i * 3 + 0];
		unsigned short index_1 = indices[i * 3 + 1];
		unsigned short index_2 = indices[i * 3 + 2];

		//���_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices[index_0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index_1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index_2].pos);

		//p0->p1,p0->p2�x�N�g�����v�Z
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//�O�ς͗������璼�p�ȃx�N�g��
		XMVECTOR normal = XMVector3Cross(v1, v2);

		//���K��
		normal = XMVector3Normalize(normal);

		//���߂��@���𒸓_�f�[�^�ɑ��
		XMStoreFloat3(&vertices[index_0].normal, normal);
		XMStoreFloat3(&vertices[index_1].normal, normal);
		XMStoreFloat3(&vertices[index_2].normal, normal);
	}

	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̐���
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	//�C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB), // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//�C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	memcpy(indexMap, indices, sizeof(indices));
	indexBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
}

void NormalMap::StaticInitialize(ID3D12Device* device)
{
	NormalMap::device = device;

	//���ʃf�[�^����
	CommonCreate();

	//�p�C�v���C���ݒ�
	Pipeline();

	pipelineState->SetName(L"NormalMapPipe");
	rootSignature->SetName(L"NormalMapRoot");
}

void NormalMap::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	NormalMap::cmdList = cmdList;

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

void NormalMap::PostDraw()
{
	// �R�}���h���X�g������
	NormalMap::cmdList = nullptr;
}

void NormalMap::Update(Camera* camera)
{
	//���[���h�s��ϊ�
	XMMATRIX matWorld = XMMatrixIdentity();
	XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matWorld *= matScale;

	XMMATRIX matRot;//�p�x
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.y));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.z));
	matWorld *= matRot;

	XMMATRIX matTrans;//���s����
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);
	matWorld *= matTrans;

	//�萔�o�b�t�@�Ƀf�[�^��]��
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);//�}�b�s���O
	constMap->color1 = color1;
	constMap->color2 = color2;
	constMap->color3 = color3;
	constMap->matWorld = matWorld;
	constMap->matView = camera->GetView();
	constMap->maProjection = camera->GetProjection();
	constMap->light = light;
	constMap->uvPos = this->uvPos;

	constBuff->Unmap(0, nullptr);
}

void NormalMap::Draw(int colorTex, int normalTex, int normalTex2)
{
	//���_�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView);

	//���_�o�b�t�@���Z�b�g
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//�萔�o�b�t�@���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
	auto heapStart = descHeap->GetGPUDescriptorHandleForHeapStart();

	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			heapStart,
			colorTex,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	cmdList->SetGraphicsRootDescriptorTable(2,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			heapStart,
			normalTex,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	cmdList->SetGraphicsRootDescriptorTable(3,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			heapStart,
			normalTex2,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//�`��R�}���h
	cmdList->DrawIndexedInstanced(36, 1, 0, 0, 0);
}