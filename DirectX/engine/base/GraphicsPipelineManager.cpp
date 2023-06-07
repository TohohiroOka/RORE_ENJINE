#include "GraphicsPipelineManager.h"
#include "SafeDelete.h"
#include "Helpar.h"
#include <string>

using namespace Microsoft::WRL;

ID3D12Device* GraphicsPipelineManager::device = nullptr;
std::unordered_map<std::string, GraphicsPipelineManager::GRAPHICS_PIPELINE> GraphicsPipelineManager::graphicsPipeline;
GraphicsPipelineManager::DrawSet GraphicsPipelineManager::oldDraw;

void SetLayout(D3D12_INPUT_ELEMENT_DESC* inputLayout,const std::vector<std::string>& _inputLayoutType)
{
	const D3D12_INPUT_CLASSIFICATION input = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

	//layout数
	const int size = int(_inputLayoutType.size());

	// 頂点レイアウトの設定
	for (int i = 0; i < size; i++)
	{
		//座標
		if (_inputLayoutType[i] == "POSITION")
		{
			inputLayout[i] = {
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//法線
		else if (_inputLayoutType[i] == "NORMAL")
		{
			inputLayout[i] = {
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//2Duv座標
		else if (_inputLayoutType[i] == "TEXCOORD_2D")
		{
			inputLayout[i] = {
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//ボーン番号
		else if (_inputLayoutType[i] == "BONEINDICES")
		{
			inputLayout[i] = {
				"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//ボーン影響度
		else if (_inputLayoutType[i] == "BONEWEIGHTS")
		{
			inputLayout[i] = {
				"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//スケール
		else if (_inputLayoutType[i] == "SCALE")
		{
			inputLayout[i] = {
				"SCALE", 0, DXGI_FORMAT_R32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//色
		else if (_inputLayoutType[i] == "COLOR")
		{
			inputLayout[i] = {
				"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
	}
}

D3D12_RENDER_TARGET_BLEND_DESC GraphicsPipelineManager::CreateBlendDesc(const std::string& _mode)
{
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc;
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA全てのチャンネルを描画
	if (_mode == "NOBLEND")
	{
		blenddesc.BlendEnable = false;
		return blenddesc;
	}

	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	if (_mode == "ALPHA")
	{
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	} else if (_mode == "ADD")
	{
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
	} else if (_mode == "SUB")
	{
		blenddesc.BlendOp = D3D12_BLEND_OP_SUBTRACT;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
	} else if (_mode == "MULA")
	{
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ZERO;
		blenddesc.DestBlend = D3D12_BLEND_INV_DEST_COLOR;
	} else if (_mode == "INVSRC")
	{
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
	}
	blenddesc.LogicOpEnable = false;

	return blenddesc;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPipelineManager::CreatepelineDesc(const PEPELINE& _pepeline)
{
	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//シェーダーのセット
	for (auto& i : _pepeline.shaderType) {
		if (i == "v") {
			gpipeline.VS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObject[_pepeline.name + "VS"].Get());
		} else if (i == "p") {
			gpipeline.PS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObject[_pepeline.name + "PS"].Get());
		} else if (i == "g") {
			gpipeline.GS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObject[_pepeline.name + "GS"].Get());
		}
	}

	//サンプルマスク設定
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//標準設定
	//ラスタライザステート設定
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//デプスステンシルステートの設定
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = CreateBlendDesc(_pepeline.blendMode);
	for (int i = 0; i < _pepeline.rtvNum; i++)
	{
		gpipeline.BlendState.RenderTarget[i] = blenddesc;
	}

	//深度値フォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//頂点レイアウトの設定
	D3D12_INPUT_ELEMENT_DESC* inputLayout = new D3D12_INPUT_ELEMENT_DESC[int(_pepeline.inputLayoutType.size())];
	SetLayout(inputLayout, _pepeline.inputLayoutType);
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements =UINT(_pepeline.inputLayoutType.size());

	//図形の形状を三角形に設定
	if (_pepeline.drawType == "POINT") {
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	}else if (_pepeline.drawType == "LINE") {
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	}else if (_pepeline.drawType == "TRIANGLE") {
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}

	//その他の設定
	gpipeline.NumRenderTargets = _pepeline.rtvNum;	//描画対象は1つ
	for (int i = 0; i < _pepeline.rtvNum; i++) {
		gpipeline.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;	//0～255指定のRGBA
	}
	gpipeline.SampleDesc.Count = 1;	//1ピクセルにつき1回サンプリング

	//2D描画なら上書きモードに設定
	if (_pepeline.drawMode == "2D")
	{
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		gpipeline.DepthStencilState.DepthEnable = false;
	} else if (_pepeline.drawMode == "PARTICLE")
	{
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	}

	return gpipeline;
}

void GraphicsPipelineManager::CreateRootSignature(const std::string& _pepelineName, const PEPELINE& _pepeline)
{
	HRESULT result = S_FALSE;

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc;

	std::vector<CD3DX12_ROOT_PARAMETER> rootparams(_pepeline.rootparams + _pepeline.textureNum);
	for (int i = 0; i < _pepeline.rootparams; i++) {
		rootparams[i].InitAsConstantBufferView(i, 0, D3D12_SHADER_VISIBILITY_ALL);
	}

	// デスクリプタレンジ
	const int tex_num = _pepeline.textureNum;
	CD3DX12_DESCRIPTOR_RANGE* descRangeSRV = new CD3DX12_DESCRIPTOR_RANGE[tex_num];
	for (int i = 0; i < tex_num; i++)
	{
		// デスクリプタレンジ
		descRangeSRV[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i); //t番 レジスタ

		// SRV（テクスチャ）
		int paramNum = _pepeline.rootparams + i;
		rootparams[paramNum].InitAsDescriptorTable(1, &descRangeSRV[i], D3D12_SHADER_VISIBILITY_ALL);
	}

	//テクスチャサンプラーの設定
	if (_pepeline.drawMode == "2D"){
		samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);
	} else {
		samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 1);
	}
	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init_1_0((UINT)rootparams.size(), rootparams.data(), 1,
		&samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//ルートシグネチャの生成
	ComPtr<ID3DBlob> rootSigBlob;

	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, nullptr);
	if (FAILED(result)) { assert(0); }

	//ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&graphicsPipeline[_pepelineName].rootSignature));
	if (FAILED(result)) { assert(0); }

}

GraphicsPipelineManager::GraphicsPipelineManager()
{
	//シェーダーのコンパイル
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

void GraphicsPipelineManager::CreatePipeline(const std::string& _name, const PEPELINE& _pepeline, GraphicsPipelineManager::DrawSet& _drawSet)
{
	HRESULT result = S_FALSE;

	std::string pepelineName = _name + _pepeline.blendMode;

	//同じキーがあればエラーを出力
	assert(!graphicsPipeline.count(_name));
	size_t L_size = graphicsPipeline.size() + 1;
	graphicsPipeline.reserve(L_size);

	//グラフィックスパイプラインの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC L_gpipeline = CreatepelineDesc(_pepeline);

	//ルートシグネチャの生成
	CreateRootSignature(pepelineName,_pepeline);

	//パイプラインデスクにルートシグネチャを登録
	L_gpipeline.pRootSignature = graphicsPipeline[pepelineName].rootSignature.Get();

	// グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&L_gpipeline, IID_PPV_ARGS(&graphicsPipeline[pepelineName].pipelineState));
	if (FAILED(result)) { assert(0); }

	graphicsPipeline[pepelineName].pipelineState->SetName(GetName(pepelineName, "PipelineState"));
	graphicsPipeline[pepelineName].rootSignature->SetName(GetName(pepelineName, "RootSignature"));

	_drawSet.pipeName= pepelineName;
	if (_pepeline.drawType == "POINT") {
		_drawSet.drawType = DrawType::POINT;
	} else if (_pepeline.drawType == "LINE") {
		_drawSet.drawType = DrawType::LINE;
	} else if (_pepeline.drawType == "TRIANGLE") {
		_drawSet.drawType = DrawType::TRIANGLE;
	}
	_drawSet.constBuffNum = _pepeline.rootparams;
	_drawSet.texNum = _pepeline.textureNum;
}

void GraphicsPipelineManager::PreDraw(ID3D12GraphicsCommandList* _cmdList,const DrawSet& _drawSet)
{
	if (_drawSet.drawType == oldDraw.drawType && _drawSet.pipeName == oldDraw.pipeName) {return;}

	oldDraw = _drawSet;

	// パイプラインステートの設定
	_cmdList->SetPipelineState(graphicsPipeline[_drawSet.pipeName].pipelineState.Get());

	// ルートシグネチャの設定
	_cmdList->SetGraphicsRootSignature(graphicsPipeline[_drawSet.pipeName].rootSignature.Get());

	// プリミティブ形状を設定
	if (_drawSet.drawType == DrawType::POINT) {
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	} else if (_drawSet.drawType == DrawType::LINE) {
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	} else if (_drawSet.drawType == DrawType::TRIANGLE) {
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}

void GraphicsPipelineManager::ResetDrawSet()
{
	oldDraw.pipeName="null";
}