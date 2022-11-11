#include "GraphicsPipelineManager.h"
#include "SafeDelete.h"

#include <string>

using namespace Microsoft::WRL;

ID3D12Device* GraphicsPipelineManager::device = nullptr;

//���O����������
LPCWSTR GetName(std::string _className, std::string _setName)
{
	//���O�̌���
	std::string name = _className + _setName;
	//�傫���擾
	size_t size = name.size();
	//���O�̃T�C�Y+1�̔z��ɍ�蒼��
	LPWSTR returnName = new wchar_t[size + 1];
	std::copy(name.begin(), name.end(), returnName);
	//�z��̖���0����
	returnName[size] = 0;

	return returnName;
}

D3D12_RENDER_TARGET_BLEND_DESC GraphicsPipelineManager::CreateBlendDesc(const BLEND_MODE& _mode)
{
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc;
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA�S�Ẵ`�����l����`��
	if (_mode == BLEND_MODE::NOBLEND)
	{
		blenddesc.BlendEnable = false;
		return blenddesc;
	}

	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	if (_mode == BLEND_MODE::ALPHA)
	{
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	} else if (_mode == BLEND_MODE::ADD)
	{
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
	} else if (_mode == BLEND_MODE::SUB)
	{
		blenddesc.BlendOp = D3D12_BLEND_OP_SUBTRACT;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
	} else if (_mode == BLEND_MODE::MULA)
	{
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ZERO;
		blenddesc.DestBlend = D3D12_BLEND_INV_DEST_COLOR;
	} else if (_mode == BLEND_MODE::INVSRC)
	{
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
	}
	blenddesc.LogicOpEnable = false;

	return blenddesc;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPipelineManager::CreatepelineDesc(const PEPELINE_DESC& _pepelineDescSet)
{
	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//�V�F�[�_�[�̃Z�b�g
	if (_pepelineDescSet.vertShader != "null")
	{
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectVS[_pepelineDescSet.vertShader].Get());
	}
	if (_pepelineDescSet.pixelShader != "null")
	{
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectPS[_pepelineDescSet.pixelShader].Get());
	}
	if (_pepelineDescSet.geometryShader != "null")
	{
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectGS[_pepelineDescSet.geometryShader].Get());
	}

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//�u�����h���[�h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc = CreateBlendDesc(_pepelineDescSet.blendMode);
	for (int i = 0; i < _pepelineDescSet.stateNum; i++)
	{
		gpipeline.BlendState.RenderTarget[i] = blenddesc;
	}

	gpipeline.NumRenderTargets = _pepelineDescSet.rtvNum;    // �`��Ώۂ�1��
	for (int i = 0; i < _pepelineDescSet.rtvNum; i++)
	{
		gpipeline.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	}
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// �}�`�̌`��ݒ�
	gpipeline.PrimitiveTopologyType = _pepelineDescSet.topologyType;

	//2D�`��Ȃ�㏑�����[�h�ɐݒ�
	if (_pepelineDescSet.object2d)
	{
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		if (_pepelineDescSet.particl)
		{
			gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		}
		else
		{
			gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			gpipeline.DepthStencilState.DepthEnable = false;
		}
	}

	gpipeline.InputLayout.pInputElementDescs = _pepelineDescSet.inputLayout;
	gpipeline.InputLayout.NumElements = _pepelineDescSet.layoutNum;

	return gpipeline;
}

void GraphicsPipelineManager::CreateRootSignature(const SIGNATURE_DESC& _signatureDescSet)
{
	HRESULT result = S_FALSE;

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc;

	// ���[�g�p�����[�^
	const int rootparam_num = 1 + (_signatureDescSet.materialData + _signatureDescSet.light +
		_signatureDescSet.instanceDraw) + _signatureDescSet.textureNum;

	std::vector<CD3DX12_ROOT_PARAMETER> rootparams(rootparam_num);
	// CBV�i���W�ϊ��s��p�j
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

	//2d�`��
	if (_signatureDescSet.object2d)
	{
		// �f�X�N���v�^�����W
		const int tex_num = _signatureDescSet.textureNum;
		CD3DX12_DESCRIPTOR_RANGE* descRangeSRV = new CD3DX12_DESCRIPTOR_RANGE[tex_num];

		for (int i = 0; i < tex_num; i++)
		{
			// �f�X�N���v�^�����W
			descRangeSRV[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i); // t0 ���W�X�^


			// SRV�i�e�N�X�`���j
			int paramNum = 1 + i;
			rootparams[paramNum].InitAsDescriptorTable(1, &descRangeSRV[i], D3D12_SHADER_VISIBILITY_ALL);
		}

		//�T���v���[�ݒ�
		samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);
	}
	//3d�`��
	else
	{
		int rootNum = 1;
		if (_signatureDescSet.materialData)
		{
			// CBV�i�}�e���A���f�[�^�p�j
			rootparams[rootNum].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
			rootNum++;
		}
		if (_signatureDescSet.light)
		{
			// CBV (���C�g)
			rootparams[rootNum].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
			rootNum++;
		}
		if (_signatureDescSet.instanceDraw)
		{
			// �C���X�^���V���O�`��p���
			rootparams[rootNum].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
			rootNum++;
		}

		// �f�X�N���v�^�����W
		const int tex_num = _signatureDescSet.textureNum;
		CD3DX12_DESCRIPTOR_RANGE* descRangeSRV = new CD3DX12_DESCRIPTOR_RANGE[tex_num];

		for (int i = 0; i < tex_num; i++)
		{
			// �f�X�N���v�^�����W
			descRangeSRV[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i); // t0 ���W�X�^

			// SRV�i�e�N�X�`���j
			int paramNum = rootNum + i;
			rootparams[paramNum].InitAsDescriptorTable(1, &descRangeSRV[i], D3D12_SHADER_VISIBILITY_ALL);
		}

		//�T���v���[�ݒ�
		samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 1);
	}

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0((UINT)rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, nullptr);
	if (FAILED(result)) { assert(0); }

	// ���[�g�V�O�l�`���̐���
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&graphicsPipeline[name].rootSignature));
	if (FAILED(result)) { assert(0); }
}

GraphicsPipelineManager::GraphicsPipelineManager()
{
	//�V�F�[�_�[�̃R���p�C��
	shaderManager = ShaderManager::Create();
}

GraphicsPipelineManager::~GraphicsPipelineManager()
{
	for (auto& pipeline : graphicsPipeline)
	{
		pipeline.second.pipelineState.Reset();
		pipeline.second.rootSignature.Reset();
	}
	shaderManager.reset();
}

void GraphicsPipelineManager::CreatePipeline(const std::string& _name, const PEPELINE_DESC& _pepelineDescSet,
	const SIGNATURE_DESC& _signatureDescSet)
{
	HRESULT result = S_FALSE;

	this->name = _name;

	//�����L�[������΃G���[���o��
	assert(!graphicsPipeline.count(name));
	size_t L_size = graphicsPipeline.size() + 1;
	graphicsPipeline.reserve(L_size);

	//�O���t�B�b�N�X�p�C�v���C���̐ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC L_gpipeline = CreatepelineDesc(_pepelineDescSet);

	//���[�g�V�O�l�`���̐���
	CreateRootSignature(_signatureDescSet);

	//�p�C�v���C���f�X�N�Ƀ��[�g�V�O�l�`����o�^
	L_gpipeline.pRootSignature = graphicsPipeline[name].rootSignature.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = device->CreateGraphicsPipelineState(&L_gpipeline, IID_PPV_ARGS(&graphicsPipeline[name].pipelineState));
	if (FAILED(result)) { assert(0); }

	graphicsPipeline[name].pipelineState->SetName(GetName(name, "PipelineState"));
	graphicsPipeline[name].rootSignature->SetName(GetName(name, "RootSignature"));
}