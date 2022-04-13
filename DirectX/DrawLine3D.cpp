#include "DrawLine3D.h"
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

ID3D12Device* DrawLine3D::device = nullptr;//�f�o�C�X
ID3D12GraphicsCommandList* DrawLine3D::cmdList = nullptr;//�R�}���h���X�g
ComPtr<ID3D12PipelineState> DrawLine3D::pipelineState;//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
ComPtr<ID3D12RootSignature> DrawLine3D::rootSignature;//���[�g�V�O�l�`��

const float PI = 3.141592f;

DrawLine3D::~DrawLine3D()
{
	//�o�b�t�@�����
	vertBuff.Reset();
	indexBuff.Reset();
	constBuff.Reset();
}

void DrawLine3D::AllDelete()
{
	//���[�g�V�O�l�`���ƃp�C�v���C���X�e�[�g���
	pipelineState.Reset();
	rootSignature.Reset();
}

void DrawLine3D::Pipeline()
{
	HRESULT result;
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	//���_�V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/DrawLine3DVS.hlsl",  // �V�F�[�_�t�@�C����
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
		L"Resources/Shaders/DrawLine3DPS.hlsl",   // �V�F�[�_�t�@�C����
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
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0���W�X�^

	//���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0);//�萔�o�b�t�@�r���[�Ƃ��ď�����
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

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

DrawLine3D* DrawLine3D::Create(UINT LineNum)
{
	HRESULT result = S_FALSE;

	DrawLine3D* instance = new DrawLine3D();;
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	if (!instance->Initialize(LineNum)) {
		delete instance;
		assert(0);
		return nullptr;
	}

	instance->Update(NULL);

	return instance;
}

void DrawLine3D::StaticInitialize(ID3D12Device* device)
{
	DrawLine3D::device = device;

	//�p�C�v���C���ݒ�
	Pipeline();

	pipelineState->SetName(L"DrawLine3dPipe");
	rootSignature->SetName(L"DrawLine3dRoot");
}

bool DrawLine3D::Initialize(UINT LineNum)
{
	HRESULT result = S_FALSE;

	//���_�f�[�^�̗v�f��
	VERTEX_ARRAY_NUM = vertNum * LineNum;
	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	const UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * VERTEX_ARRAY_NUM);
	//���_�f�[�^�̗v�f���ύX
	vertices.resize(VERTEX_ARRAY_NUM);

	//���_�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	//���_�o�b�t�@�r���[�̐���
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	//�C���f�b�N�z��̗v�f��
	INDEX_ARRAY_NUM = indexNum * LineNum;
	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	const UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * INDEX_ARRAY_NUM);

	//�g���C���f�b�N�X�̍쐬
	std::vector<unsigned short> indices;
	for (int i = 0; i < INDEX_ARRAY_NUM; i++)
	{
		//���C���f�b�N�X
		unsigned short addIndex;

		//�x�[�X + ��{�Ŏg���C���f�b�N�X�ԍ��̍ő�l * (���݂̔z�� / ��{���̔z��̍ő吔)
		addIndex = BASE_INDICES[i % indexNum] + 4 * (int)(i / indexNum);

		//�z��̈�Ԍ��ɓ����
		indices.push_back(addIndex);
	}

	//�C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB), // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	//�C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	std::copy(indices.begin(), indices.end(), indexMap);
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
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	return true;
}

float DrawLine3D::GetAngle(XMFLOAT3 startPoint, XMFLOAT3 endPoint) {
	float angle = atan2f(endPoint.y - startPoint.y, endPoint.x - startPoint.x) * (180.0f / PI);
	return angle;
}

void DrawLine3D::SetLine(XMFLOAT3 startPoint[], XMFLOAT3 endPoint[], float width)
{
	HRESULT result;

	//��
	XMFLOAT2 lineWidth1 = {};
	XMFLOAT2 lineWidth2 = {};

	//���̖{������
	const int LineNum = VERTEX_ARRAY_NUM / vertNum;
	for (int i = 0; i < LineNum; i++)
	{
		//�p�x
		const float angle = GetAngle(startPoint[i], endPoint[i]);

		//�������p�l
		float LEFT = (angle + 90.0f) * (PI / 180.0f);
		float RIGHT = (angle - 90.0f) * (PI / 180.0f);

		lineWidth1.x = width * cosf(LEFT);
		lineWidth1.y = width * sinf(LEFT);
		lineWidth2.x = width * cosf(RIGHT);
		lineWidth2.y = width * sinf(RIGHT);

		// ���_�f�[�^
		int arrayNum = i * 4;
		vertices[arrayNum].pos = { endPoint[i].x + lineWidth2.x, endPoint[i].y + lineWidth2.y, endPoint[i].z }; // ����
		arrayNum++;
		vertices[arrayNum].pos = { startPoint[i].x + lineWidth2.x, startPoint[i].y + lineWidth2.y, startPoint[i].z }; // ����
		arrayNum++;
		vertices[arrayNum].pos = { endPoint[i].x + lineWidth1.x, endPoint[i].y + lineWidth1.y, endPoint[i].z }; // �E��
		arrayNum++;
		vertices[arrayNum].pos = { startPoint[i].x + lineWidth1.x, startPoint[i].y + lineWidth1.y, startPoint[i].z }; // �E��
	}

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}
}

void DrawLine3D::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	DrawLine3D::cmdList = cmdList;

	//�p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelineState.Get());

	//���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(rootSignature.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void DrawLine3D::PostDraw()
{
	// �R�}���h���X�g������
	DrawLine3D::cmdList = nullptr;
}

void DrawLine3D::Update(Camera* camera)
{
	//���[���h�s��ϊ�
	XMMATRIX matWorld = XMMatrixIdentity();
	XMMATRIX matScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	matWorld *= matScale;

	XMMATRIX matRot;//�p�x
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(0.0f));
	matRot *= XMMatrixRotationX(XMConvertToRadians(0.0f));
	matRot *= XMMatrixRotationY(XMConvertToRadians(0.0f));
	matWorld *= matRot;

	XMMATRIX matTrans;//���s����
	matTrans = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	matWorld *= matTrans;

	//�萔�o�b�t�@�Ƀf�[�^��]��
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);//�}�b�s���O
	constMap->color = color;
	constMap->matWorld = matWorld;
	if (camera != nullptr)
	{
		constMap->matView = camera->GetView();
		constMap->maProjection = camera->GetProjection();
	} else {
		constMap->matView = XMMatrixIdentity();
		constMap->maProjection = XMMatrixIdentity();
	}

	constBuff->Unmap(0, nullptr);
}

void DrawLine3D::Draw()
{
	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView);

	//���_�o�b�t�@���Z�b�g
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//�萔�o�b�t�@���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//�`��R�}���h
	cmdList->DrawIndexedInstanced(INDEX_ARRAY_NUM, 1, 0, 0, 0);
}