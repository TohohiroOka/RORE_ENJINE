#include "GraphicsPipelineManager.h"
#include "SafeDelete.h"

#include <string>

using namespace Microsoft::WRL;

ID3D12Device* GraphicsPipelineManager::device = nullptr;
std::unique_ptr<ShaderManager> GraphicsPipelineManager::shaderManager = nullptr;
D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPipelineManager::defaultGpipeline{};
D3D12_RENDER_TARGET_BLEND_DESC GraphicsPipelineManager::blenddesc{};

//名前を結合する
LPCWSTR GetName(std::string className, std::string setName)
{
	//名前の結合
	std::string name = className + setName;
	//大きさ取得
	size_t size = name.size();
	//名前のサイズ+1の配列に作り直す
	LPWSTR returnName = new wchar_t[size + 1];
	std::copy(name.begin(), name.end(), returnName);
	//配列の末に0を代入
	returnName[size] = 0;

	return returnName;
}

GraphicsPipelineManager::~GraphicsPipelineManager()
{
	rootSignature.Reset();
	pipelineState.Reset();
}

void GraphicsPipelineManager::StaticInitialize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!GraphicsPipelineManager::device);

	// nullptrチェック
	assert(device);

	//デバイスの登録
	GraphicsPipelineManager::device = device;

	//シェーダーのコンパイル
	shaderManager = ShaderManager::Create();

	//パイプラインの基盤生成
	CreateDefaultGpipeline();
}

void GraphicsPipelineManager::CreateDefaultGpipeline()
{
	// サンプルマスク
	defaultGpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	defaultGpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// デプスステンシルステート
	defaultGpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	defaultGpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	defaultGpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 図形の形状設定（三角形）
	defaultGpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	defaultGpipeline.NumRenderTargets = 1;    // 描画対象は1つ
	defaultGpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	defaultGpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング
}

std::unique_ptr<GraphicsPipelineManager> GraphicsPipelineManager::Create(const std::string name, const unsigned char objectKind,
	const D3D12_INPUT_ELEMENT_DESC* inputLayout, const UINT inputLayoutSize)
{
	// 3Dオブジェクトのインスタンスを生成
	GraphicsPipelineManager* instance = new GraphicsPipelineManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->CreatePipeline(name, objectKind, inputLayout, inputLayoutSize);

	return std::unique_ptr<GraphicsPipelineManager>(instance);
}

void GraphicsPipelineManager::CreatePipeline(const std::string name, const unsigned char objectKind,
	const D3D12_INPUT_ELEMENT_DESC* inputLayout, const UINT inputLayoutSize)
{
	HRESULT result = S_FALSE;

	//オブジェクトの種類を登録
	this->objectKind = objectKind;

	//グラフィックスパイプラインの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = CreatepelineDesc(inputLayout, inputLayoutSize);

	//ルートシグネチャの生成
	CreateRootSignature();

	//パイプラインデスクにルートシグネチャを登録
	gpipeline.pRootSignature = rootSignature.Get();

	// グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineState));
	if (FAILED(result)) {
		assert(0);
	}

	pipelineState->SetName(GetName(name, "PipelineState"));
	rootSignature->SetName(GetName(name, "RootSignature"));
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPipelineManager::CreatepelineDesc(
	const D3D12_INPUT_ELEMENT_DESC* inputLayout, const UINT inputLayoutSize)
{
	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = defaultGpipeline;

	//Obj
	if (objectKind == OBJECT_KINDS::OBJ)
	{
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectVS["OBJ"].Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectPS["OBJ"].Get());

		// ブレンドステートの設定（ポストエフェクト分を追加で生成）
		gpipeline.BlendState.RenderTarget[1] = blenddesc;
		gpipeline.BlendState.RenderTarget[2] = blenddesc;

		gpipeline.NumRenderTargets = 3;    // 描画対象は1つ
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	}
	//Fbx
	else if (objectKind == OBJECT_KINDS::FBX)
	{
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectVS["FBX"].Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectPS["FBX"].Get());

		// ブレンドステートの設定（ポストエフェクト分を追加で生成）
		gpipeline.BlendState.RenderTarget[1] = blenddesc;
		gpipeline.BlendState.RenderTarget[2] = blenddesc;

		gpipeline.NumRenderTargets = 3;    // 描画対象は1つ
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	}
	//DrawLine3d
	else if (objectKind == OBJECT_KINDS::DRAW_LINE_3D)
	{
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectVS["DRAW_LINE_3D"].Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectPS["DRAW_LINE_3D"].Get());
	}
	//NormalMap
	else if (objectKind == OBJECT_KINDS::NORMAL_MAP)
	{
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectVS["NORMAL_MAP"].Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectPS["NORMAL_MAP"].Get());
	}
	//Sprite
	else if (objectKind == OBJECT_KINDS::SPRITE)
	{
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectVS["SPRITE"].Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectPS["SPRITE"].Get());

		//常に三角形描画
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		//常に上書きルール
		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		gpipeline.DepthStencilState.DepthEnable = false;

		// ブレンドステートの再設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
	}
	//DrawLine2d
	else if (objectKind == OBJECT_KINDS::DRAW_LINE_2D)
	{
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectVS["DRAW_LINE_3D"].Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectPS["DRAW_LINE_3D"].Get());

		//常に三角形描画
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		//常に上書きルール
		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		gpipeline.DepthStencilState.DepthEnable = false;

		// ブレンドステートの再設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
	}
	//PostEffect
	else if (objectKind == OBJECT_KINDS::POST_EFFECT)
	{
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectVS["POST_EFFECT"].Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectPS["POST_EFFECT"].Get());

		//常に三角形描画
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		//常に上書きルール
		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		gpipeline.DepthStencilState.DepthEnable = false;

		// ブレンドステートの再設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
	}
	//Particle
	else if (objectKind == OBJECT_KINDS::PARTICLE)
	{
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectVS["PARTICLE"].Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectPS["PARTICLE"].Get());
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectGS["PARTICLE"].Get());

		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		// 加算ブレンディング
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		// ブレンドステートの再設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
	}
	//CubeBox
	else if (objectKind == OBJECT_KINDS::CUBE_BOX)
	{
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectVS["CUBE_BOX"].Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(shaderManager->shaderObjectPS["CUBE_BOX"].Get());
	}

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = inputLayoutSize;

	return gpipeline;
}

void GraphicsPipelineManager::CreateRootSignature()
{
	HRESULT result = S_FALSE;

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ
	// ルートパラメータ
	std::vector<CD3DX12_ROOT_PARAMETER> rootparams;
	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

	//Obj
	if (objectKind == OBJECT_KINDS::OBJ)
	{
		// ルートパラメータ
		rootparams.resize(5);
		// CBV（座標変換行列用）
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（マテリアルデータ用）
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ）
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		// CBV (ライト)
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	}
	//Fbx
	else if (objectKind == OBJECT_KINDS::FBX)
	{
		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV_t1;
		descRangeSRV_t1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1 レジスタ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV_t2;
		descRangeSRV_t2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); // t2 レジスタ

		// ルートパラメータ
		rootparams.resize(7);
		// CBV（座標変換行列用）
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（マテリアルデータ用）
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ）
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		// CBV (ライト)
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV (スキニング用)
		rootparams[4].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（キューブマップテクスチャ）
		rootparams[5].InitAsDescriptorTable(1, &descRangeSRV_t1, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（キューブマップテクスチャ）
		rootparams[6].InitAsDescriptorTable(1, &descRangeSRV_t2, D3D12_SHADER_VISIBILITY_ALL);
	}
	//DrawLine3d
	else if (objectKind == OBJECT_KINDS::DRAW_LINE_3D)
	{
		// ルートパラメータ
		rootparams.resize(2);
		// CBV（座標変換行列用）
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ）
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	}
	//NormalMap
	else if (objectKind == OBJECT_KINDS::NORMAL_MAP)
	{
		//追加のデスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE addDescRangeSRV[2] = {};
		addDescRangeSRV[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//t0レジスタ
		addDescRangeSRV[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);//t0レジスタ

		// ルートパラメータ
		rootparams.resize(4);
		// CBV（座標変換行列用）
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ1）
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ2）
		rootparams[2].InitAsDescriptorTable(1, &addDescRangeSRV[0], D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ3）
		rootparams[3].InitAsDescriptorTable(1, &addDescRangeSRV[1], D3D12_SHADER_VISIBILITY_ALL);
	}
	//Sprite
	else if (objectKind == OBJECT_KINDS::SPRITE)
	{
		// ルートパラメータ
		rootparams.resize(2);
		// CBV（座標変換行列用）
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ1）
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		
		// スタティックサンプラーのs0レジスタ
		samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);
	}
	//DrawLine2d
	else if (objectKind == OBJECT_KINDS::DRAW_LINE_2D)
	{
		// ルートパラメータ
		rootparams.resize(2);
		// CBV（座標変換行列用）
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ1）
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

		// スタティックサンプラーのs0レジスタ
		samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);
	}
	//PostEffect
	else if (objectKind == OBJECT_KINDS::POST_EFFECT)
	{
		//追加のデスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE addDescRangeSRV1;
		addDescRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//t1レジスタ
		CD3DX12_DESCRIPTOR_RANGE addDescRangeSRV2;
		addDescRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);//t2レジスタ

		// ルートパラメータ
		rootparams.resize(4);
		// CBV（座標変換行列用）
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ1）
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ2）
		rootparams[2].InitAsDescriptorTable(1, &addDescRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ3）
		rootparams[3].InitAsDescriptorTable(1, &addDescRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);

		// スタティックサンプラーのs0レジスタ
		samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);
	}
	//Particle
	else if (objectKind == OBJECT_KINDS::PARTICLE)
	{
		// ルートパラメータ
		rootparams.resize(2);
		// CBV（座標変換行列用）
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ1）
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	}
	//CubeBox
	else if (objectKind == OBJECT_KINDS::CUBE_BOX)
	{
		// ルートパラメータ
		rootparams.resize(2);
		// CBV（座標変換行列用）
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ1）
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	}

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0((UINT)rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, nullptr);
	if (FAILED(result)) {
		assert(0);
	}

	// ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	if (FAILED(result)) {
		assert(0);
	}
}

void GraphicsPipelineManager::Finalize()
{
	shaderManager.reset();
}