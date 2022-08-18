#include "PostEffect.h"
#include "WindowApp.h"
#include "DirectInput.h"
#include "InterfaceObject3d.h"

using namespace DirectX;
using namespace Microsoft::WRL;

const float PostEffect::clearColor[2][4] = {
	{ 0.1f,0.1f,0.7f,0.0f },
	{ 0.0f,0.0f,0.0f,0.0f }
};
GraphicsPipelineManager::GRAPHICS_PIPELINE PostEffect::pipeline;

PostEffect::PostEffect()
	:Sprite()
{
}

PostEffect::~PostEffect()
{
}

void PostEffect::Finalize()
{
	//pipeline.reset();
	descHeapRTV.Reset();
	descHeapDSV.Reset();
}

void PostEffect::StaticInitialize()
{
}

void PostEffect::Initialize()
{
	HRESULT result;

	//���_�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// ���_�f�[�^
	VertexPosUv vertices[vertNum] = {
		{{ -1,-1,0.0f }, {0.0f,1.0f}}, // ����
		{{ -1,+1,0.0f }, {0.0f,0.0f}}, // ����
		{{ +1,-1,0.0f }, {1.0f,1.0f}}, // �E��
		{{ +1,+1,0.0f }, {1.0f,0.0f}}, // �E��
	};

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	// ���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * 4;
	vbView.StrideInBytes = sizeof(VertexPosUv);

	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	//�e�N�X�`���o�b�t�@�����p�ϐ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		WindowApp::GetWindowWidth(), (UINT)WindowApp::GetWindowHeight(),
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	//�[�x�ȊO�̃e�N�X�`������
	const int breakNum = TEX_TYPE::SIZE - 1;

	for (int i = 0; i < breakNum; i++)
	{
		// �e�N�X�`���p�o�b�t�@�̐���
		texture[i] = std::make_unique<Texture>();

		int clearColorNum = i != TEX_TYPE::NORMAL;

		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // �e�N�X�`���p�w��
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor[clearColorNum]),
			IID_PPV_ARGS(&texture[i]->texBuffer));
		assert(SUCCEEDED(result));

		//�e�N�X�`����ԃN���A
		{
			//��f��
			const UINT pixelCount = WindowApp::GetWindowWidth() * WindowApp::GetWindowHeight();
			//��f��s���̃f�[�^�T�C�Y
			const UINT rowPitch = sizeof(UINT) * WindowApp::GetWindowWidth();
			//��f�S�̂̃f�[�^�T�C�Y
			const UINT depthPitch = rowPitch * WindowApp::GetWindowHeight();
			//��f�C���[�W
			UINT* img = new UINT[pixelCount];
			for (UINT j = 0; j < pixelCount; j++) { img[j] = 0xff0000ff; }

			// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
			result = texture[i]->texBuffer->WriteToSubresource(0, nullptr,
				img, rowPitch, depthPitch);
			assert(SUCCEEDED(result));
			delete[] img;
		}
	}

	//SRV�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < breakNum; i++)
	{
		//�f�X�N���v�^�q�[�v��SRV����
		texture[i]->descriptor = std::make_unique<DescriptorHeapManager>();
		texture[i]->descriptor->CreateSRV(texture[i]->texBuffer, srvDesc);
	}

	//RTV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHescDesc{};
	rtvDescHescDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHescDesc.NumDescriptors = breakNum;

	//RTV�p�f�X�N���v�^�q�[�v�𐶐�
	result = device->CreateDescriptorHeap(&rtvDescHescDesc, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));

	//�f�X�N���v�^�q�[�v��RTV����
	for (int i = 0; i < breakNum; i++)
	{
		device->CreateRenderTargetView(texture[i]->texBuffer.Get(), nullptr,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeapRTV->GetCPUDescriptorHandleForHeapStart(), i,
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));
	}
}

void PostEffect::CreateDepthBuffer()
{
	HRESULT result;

	//�[�x�o�b�t�@���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R32_TYPELESS,
			WindowApp::GetWindowWidth(),
			(UINT)WindowApp::GetWindowHeight(),
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	//�[�x�o�b�t�@�̐���
	texture[TEX_TYPE::DEPTH] = std::make_unique<Texture>();
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//�[�x�l�������݂Ɏg�p
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&texture[TEX_TYPE::DEPTH]->texBuffer));
	assert(SUCCEEDED(result));

	//DSV�p�f�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//�f�v�X�X�e���V���r���[
	dsvHeapDesc.NumDescriptors = 1;//�[�x�r���[��1��

	//�f�X�N���v�^�q�[�v��DSV����
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));

	//�[�x�o�b�t�@���\�[�X�ݒ�
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(texture[TEX_TYPE::DEPTH]->texBuffer.Get(), &dsvDesc, descHeapDSV->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//�f�X�N���v�^�q�[�v��SRV����
	texture[TEX_TYPE::DEPTH]->descriptor = std::make_unique<DescriptorHeapManager>();
	texture[TEX_TYPE::DEPTH]->descriptor->CreateSRV(texture[TEX_TYPE::DEPTH]->texBuffer, srvDesc);
}

std::unique_ptr<PostEffect> PostEffect::Create()
{
	// Sprite�̃C���X�^���X�𐶐�
	PostEffect* instance = new PostEffect();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();
	//�[�x�o�b�t�@����
	instance->CreateDepthBuffer();

	//���j�[�N�|�C���^��Ԃ�
	return std::unique_ptr<PostEffect>(instance);
}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->outlineColor = InterfaceObject3d::GetOutlineColor();
	constMap->outlineWidth = InterfaceObject3d::GetOutlineWidth();
	constBuff->Unmap(0, nullptr);

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipeline.pipelineState.Get());

	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(pipeline.rootSignature.Get());

	//�v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ���_�o�b�t�@�̐ݒ�
	cmdList->IASetVertexBuffers(0, 1, &this->vbView);

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, this->constBuff->GetGPUVirtualAddress());

	//�V�F�[�_�[���\�[�X�r���[
	for (int i = 0; i < TEX_TYPE::SIZE; i++)
	{
		cmdList->SetGraphicsRootDescriptorTable(i + 1, texture[i]->descriptor->gpu);
	}

	// �`��R�}���h
	cmdList->DrawInstanced(4, 1, 0, 0);
}

void PostEffect::PreDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	const int breakNum = TEX_TYPE::SIZE - 1;

	//���\�[�X�o���A��ύX
	for (int i = 0; i < breakNum; i++)
	{
		cmdList->ResourceBarrier(1,
			&CD3DX12_RESOURCE_BARRIER::Transition(texture[i]->texBuffer.Get(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET));
	}

	//�����_�[�^�[�Q�b�g�r���[�p�f�X�N���v�^�q�[�v�̃n���h���擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHs[breakNum];
	for (int i = 0; i < breakNum; i++)
	{
		if (breakNum == i) { break; }
		rtvHs[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeapRTV->GetCPUDescriptorHandleForHeapStart(), i,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	}

	//�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h���擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	//�����_�[�^�[�Q�b�g���Z�b�g
	cmdList->OMSetRenderTargets(breakNum, rtvHs, false, &dsvH);

	//�r���[�|�[�g�ݒ�
	CD3DX12_VIEWPORT viewports[breakNum];
	CD3DX12_RECT scissorRects[breakNum];
	for (int i = 0; i < breakNum; i++)
	{
		viewports[i] = CD3DX12_VIEWPORT(0.0f, 0.0f, (FLOAT)WindowApp::GetWindowWidth(), (FLOAT)WindowApp::GetWindowHeight());
		scissorRects[i] = CD3DX12_RECT(0, 0, (LONG)WindowApp::GetWindowWidth(), (LONG)WindowApp::GetWindowHeight());
	}

	cmdList->RSSetViewports(1, viewports);
	//�V�U�����O��`�ݒ�
	cmdList->RSSetScissorRects(1, scissorRects);

	//�S��ʃN���A
	for (int i = 0; i < breakNum; i++)
	{
		int clearColorNum = i != TEX_TYPE::NORMAL;
		cmdList->ClearRenderTargetView(rtvHs[i], clearColor[clearColorNum], 0, nullptr);
	}

	//�[�x�o�b�t�@�̃N���A
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffect::PostDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	const int breakNum = TEX_TYPE::DEPTH - 1;

	//���\�[�X�o���A��ύX
	for (int i = 0; i < breakNum; i++)
	{
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texture[i]->texBuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}
}