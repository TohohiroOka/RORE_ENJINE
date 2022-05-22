#include "CubeMap.h"
#include "Camera.h"
#include <DirectXTex.h>
#include <string>
#include "SafeDelete.h"

using namespace Microsoft::WRL;
using namespace DirectX;

ID3D12Device* CubeMap::device = nullptr;
ID3D12GraphicsCommandList* CubeMap::cmdList = nullptr;
std::unique_ptr<GraphicsPipelineManager> CubeMap::pipeline = nullptr;
ComPtr<ID3D12Resource> CubeMap::texBuffer = nullptr;
ComPtr<ID3D12DescriptorHeap> CubeMap::descHeap = nullptr;
Camera* CubeMap::camera = nullptr;

void CubeMap::CreateGraphicsPipeline()
{
	////���_���C�A�E�g�z��̐錾�Ɛݒ�
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	pipeline = GraphicsPipelineManager::Create("CubeBox",
		GraphicsPipelineManager::OBJECT_KINDS::CUBE_BOX, inputLayout, _countof(inputLayout));
}

void CubeMap::StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	// �ď������`�F�b�N
	assert(!CubeMap::device);
	assert(device);
	CubeMap::device = device;

	// �ď������`�F�b�N
	assert(!CubeMap::cmdList);
	assert(cmdList);
	CubeMap::cmdList = cmdList;

	//�f�X�N���v�^�q�[�v�̐���
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 6;
	HRESULT result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));
	assert(SUCCEEDED(result));

	CreateGraphicsPipeline();
}

std::unique_ptr<CubeMap> CubeMap::Create()
{
	//�C���X�^���X�𐶐�
	CubeMap* instance = new CubeMap();
	if (instance == nullptr) {
		return nullptr;
	}

	//�e�N�X�`���ǂݍ���
	instance->LoadCubeTexture();

	//������
	instance->Initialize();

	texBuffer->SetName(L"CubeTex");
	descHeap->SetName(L"Cube_DescHeap");

	return std::unique_ptr<CubeMap>(instance);
}

void CubeMap::SetCubeMap(ID3D12GraphicsCommandList* cmdList)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		texBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);
}

void CubeMap::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	CubeMap::cmdList = cmdList;

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipeline->pipelineState.Get());

	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(pipeline->rootSignature.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�f�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void CubeMap::PostDraw()
{
	// �R�}���h���X�g������
	CubeMap::cmdList = nullptr;
}

void CubeMap::Finalize()
{
	descHeap.Reset();
	texBuffer.Reset();
	pipeline.reset();
}

void CubeMap::Initialize()
{
	HRESULT result = S_FALSE;
	//���_�̒���
	float edge = 1.0f;
	const int vertNum = 24;
	Vertex vertices[vertNum] = {
		//face1
		{{ -edge, -edge, -edge }, { -1.0f, -1.0f, -1.0f}}, // ���� / ���_1
		{{ -edge,  edge, -edge }, { -1.0f,  1.0f, -1.0f}}, // ���� / ���_2
		{{  edge, -edge, -edge }, { -1.0f, -1.0f,  1.0f}}, // �E�� / ���_3
		{{  edge,  edge, -edge }, { -1.0f,  1.0f,  1.0f}}, // �E�� / ���_4
		//face0
		{{  edge, -edge,  edge }, {  1.0f, -1.0f,  1.0f}}, // ���� / ���_5
		{{  edge,  edge,  edge }, {  1.0f,  1.0f,  1.0f}}, // ���� / ���_6
		{{ -edge, -edge,  edge }, {  1.0f, -1.0f, -1.0f}}, // �E�� / ���_7
		{{ -edge,  edge,  edge }, {  1.0f,  1.0f, -1.0f}}, // �E�� / ���_8
		//face4
		{{  edge, -edge, -edge }, { -1.0f, -1.0f,  1.0f}}, // ���� / ���_3
		{{  edge,  edge, -edge }, { -1.0f,  1.0f,  1.0f}}, // ���� / ���_4
		{{  edge, -edge,  edge }, {  1.0f, -1.0f,  1.0f}}, // �E�� / ���_5
		{{  edge,  edge,  edge }, {  1.0f,  1.0f,  1.0f}}, // �E�� / ���_6
		//face5
		{{ -edge, -edge,  edge }, {  1.0f, -1.0f, -1.0f}}, // ���� / ���_3
		{{ -edge,  edge,  edge }, {  1.0f,  1.0f, -1.0f}}, // ���� / ���_4
		{{ -edge, -edge, -edge }, { -1.0f, -1.0f, -1.0f}}, // �E�� / ���_1
		{{ -edge,  edge, -edge }, { -1.0f,  1.0f, -1.0f}}, // �E�� / ���_2
		//face2
		{{  edge,  edge, -edge }, { -1.0f,  1.0f,  1.0f}}, // ���� / ���_4
		{{ -edge,  edge, -edge }, { -1.0f,  1.0f, -1.0f}}, // ���� / ���_2
		{{  edge,  edge,  edge }, {  1.0f,  1.0f,  1.0f}}, // �E�� / ���_6
		{{ -edge,  edge,  edge }, {  1.0f,  1.0f, -1.0f}}, // �E�� / ���_8
		//face3
		{{ -edge, -edge, -edge }, { -1.0f, -1.0f, -1.0f}}, // ���� / ���_1
		{{  edge, -edge, -edge }, { -1.0f, -1.0f,  1.0f}}, // ���� / ���_3
		{{ -edge, -edge,  edge }, {  1.0f, -1.0f, -1.0f}}, // �E�� / ���_7
		{{  edge, -edge,  edge }, {  1.0f, -1.0f,  1.0f}}, // �E�� / ���_5
	};

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
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̐���
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(Vertex) * vertNum;
	vbView.StrideInBytes = sizeof(Vertex);

	unsigned short indices[indexNum] = {
		0 ,2 ,1 ,2 ,3 ,1 ,
		4 ,6 ,5 ,6 ,7 ,5 ,
		8 ,10,9 ,10,11,9 ,
		12,14,13,14,15,13,
		16,18,17,18,19,17,
		20,22,21,22,23,21
	};

	//�C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(unsigned short) * indexNum), // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	assert(SUCCEEDED(result));

	//�C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	memcpy(indexMap, indices, sizeof(indices));
	indexBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(unsigned short) * indexNum;

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

void CubeMap::LoadCubeTexture()
{
	HRESULT result = S_FALSE;

	{
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;

		result = DirectX::LoadFromDDSFile(L"Resources/CubeMap/cubemap.dds", DDS_FLAGS_NONE, &metadata, image);
		assert(SUCCEEDED(result));

		result = CreateTexture(device, metadata, &texBuffer);
		assert(SUCCEEDED(result));

		result = PrepareUpload(device, image.GetImages(), image.GetImageCount(), metadata, subresources);
		assert(SUCCEEDED(result));

		//�T�C�Y
		UINT totalBytes = (UINT)GetRequiredIntermediateSize(texBuffer.Get(), 0, UINT(subresources.size()));

		//�e�N�X�`���]���p�o�b�t�@����
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(totalBytes),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&texConstBuffer));
		assert(SUCCEEDED(result));

		UpdateSubresources(
			cmdList,
			texBuffer.Get(), texConstBuffer.Get(),
			0, 0, (UINT)subresources.size(), subresources.data());

		SetCubeMap(cmdList);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = (UINT)metadata.mipLevels;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		device->CreateShaderResourceView(texBuffer.Get(), &srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), 0,
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
	}

}

CubeMap::~CubeMap()
{
}

void CubeMap::Update()
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

void CubeMap::Draw()
{
	//�萔�o�b�t�@���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
	auto heapStart = descHeap->GetGPUDescriptorHandleForHeapStart();

	//���_�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView);

	//���_�o�b�t�@���Z�b�g
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//�e�N�X�`���o�b�t�@�]��
	TransferTextureBubber(cmdList, 1);

	//�`��R�}���h
	cmdList->DrawIndexedInstanced(indexNum, 1, 0, 0, 0);
}

void CubeMap::TransferTextureBubber(ID3D12GraphicsCommandList* cmdList,UINT RootParameterIndex)
{
	cmdList->SetGraphicsRootDescriptorTable(RootParameterIndex,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
}