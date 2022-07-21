#include "PostEffect.h"
#include "WindowApp.h"
#include "DirectInput.h"
#include "InterfaceObject3d.h"

using namespace DirectX;
using namespace Microsoft::WRL;

const float PostEffect::clearColor[4] = { 0.1f,0.1f,0.7f,0.0f };
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

	//頂点バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// 頂点データ
	VertexPosUv vertices[vertNum] = {
		{{ -1,-1,0.0f }, {0.0f,1.0f}}, // 左下
		{{ -1,+1,0.0f }, {0.0f,0.0f}}, // 左上
		{{ +1,-1,0.0f }, {1.0f,1.0f}}, // 右下
		{{ +1,+1,0.0f }, {1.0f,0.0f}}, // 右上
	};

	// 頂点バッファへのデータ転送
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * 4;
	vbView.StrideInBytes = sizeof(VertexPosUv);

	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	//テクスチャバッファ生成用変数
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		WindowApp::GetWindowWidth(), (UINT)WindowApp::GetWindowHeight(),
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	// テクスチャ用バッファの生成
	texture[TEX_TYPE::NORMAL] = std::make_unique<Texture>();

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // テクスチャ用指定
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor),
		IID_PPV_ARGS(&texture[TEX_TYPE::NORMAL]->texBuffer));
	assert(SUCCEEDED(result));

	//テクスチャを赤クリア
	{
		//画素数
		const UINT pixelCount = WindowApp::GetWindowWidth() * WindowApp::GetWindowHeight();
		//画素一行分のデータサイズ
		const UINT rowPitch = sizeof(UINT) * WindowApp::GetWindowWidth();
		//画素全体のデータサイズ
		const UINT depthPitch = rowPitch * WindowApp::GetWindowHeight();
		//画素イメージ
		UINT* img = new UINT[pixelCount];
		for (UINT j = 0; j < pixelCount; j++) { img[j] = 0xff0000ff; }

		// テクスチャバッファにデータ転送
		result = texture[TEX_TYPE::NORMAL]->texBuffer->WriteToSubresource(0, nullptr,
			img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[] img;
	}

	//SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//デスクリプタヒープにSRV生成
	texture[TEX_TYPE::NORMAL]->descriptor = std::make_unique<DescriptorHeapManager>();
	texture[TEX_TYPE::NORMAL]->descriptor->CreateSRV(texture[TEX_TYPE::NORMAL]->texBuffer, srvDesc);

	//RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHescDesc{};
	rtvDescHescDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHescDesc.NumDescriptors = 1;

	//RTV用デスクリプタヒープを生成
	result = device->CreateDescriptorHeap(&rtvDescHescDesc, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));

	//デスクリプタヒープにRTV生成
	device->CreateRenderTargetView(texture[TEX_TYPE::NORMAL]->texBuffer.Get(), nullptr,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeapRTV->GetCPUDescriptorHandleForHeapStart(), 0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));
}

void PostEffect::CreateDepthBuffer()
{
	HRESULT result;

	//深度バッファリソース設定
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R32_TYPELESS,
			WindowApp::GetWindowWidth(),
			(UINT)WindowApp::GetWindowHeight(),
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	//深度バッファの生成
	texture[TEX_TYPE::DEPTH] = std::make_unique<Texture>();
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値書き込みに使用
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&texture[TEX_TYPE::DEPTH]->texBuffer));
	assert(SUCCEEDED(result));

	//DSV用デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビュー
	dsvHeapDesc.NumDescriptors = 1;//深度ビューは1つ

	//デスクリプタヒープにDSV生成
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));

	//深度バッファリソース設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(texture[TEX_TYPE::DEPTH]->texBuffer.Get(), &dsvDesc, descHeapDSV->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//デスクリプタヒープにSRV生成
	texture[TEX_TYPE::DEPTH]->descriptor = std::make_unique<DescriptorHeapManager>();
	texture[TEX_TYPE::DEPTH]->descriptor->CreateSRV(texture[TEX_TYPE::DEPTH]->texBuffer, srvDesc);
}

std::unique_ptr<PostEffect> PostEffect::Create()
{
	// Spriteのインスタンスを生成
	PostEffect* instance = new PostEffect();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();
	//深度バッファ生成
	instance->CreateDepthBuffer();

	//ユニークポインタを返す
	return std::unique_ptr<PostEffect>(instance);
}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	HRESULT result= constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->outlineColor = InterfaceObject3d::GetOutlineColor();
	constMap->outlineWidth = InterfaceObject3d::GetOutlineWidth();
	constBuff->Unmap(0, nullptr);

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipeline.pipelineState.Get());

	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipeline.rootSignature.Get());

	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &this->vbView);

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, this->constBuff->GetGPUVirtualAddress());

	//シェーダーリソースビュー
	for (int i = 0; i < texBuffNum; i++)
	{
		cmdList->SetGraphicsRootDescriptorTable(i + 1, texture[i]->descriptor->gpu);
	}

	// 描画コマンド
	cmdList->DrawInstanced(4, 1, 0, 0);
}

void PostEffect::PreDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	//リソースバリアを変更
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(texture[TEX_TYPE::NORMAL]->texBuffer.Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET));

	//レンダーターゲットビュー用デスクリプタヒープのハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHs;
	rtvHs = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeapRTV->GetCPUDescriptorHandleForHeapStart(), 0,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	//深度ステンシルビュー用デスクリプタヒープのハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	//レンダーターゲットをセット
	cmdList->OMSetRenderTargets(1, &rtvHs, false, &dsvH);

	//ビューポート設定
	CD3DX12_VIEWPORT viewports;
	CD3DX12_RECT scissorRects;
	viewports = CD3DX12_VIEWPORT(0.0f, 0.0f, (FLOAT)WindowApp::GetWindowWidth(), (FLOAT)WindowApp::GetWindowHeight());
	scissorRects= CD3DX12_RECT(0, 0, (LONG)WindowApp::GetWindowWidth(), (LONG)WindowApp::GetWindowHeight());
	
	cmdList->RSSetViewports(1, &viewports);
	//シザリング矩形設定
	cmdList->RSSetScissorRects(1, &scissorRects);

	//全画面クリア
	cmdList->ClearRenderTargetView(rtvHs, clearColor, 0, nullptr);

	//深度バッファのクリア
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffect::PostDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	//リソースバリアを変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texture[TEX_TYPE::NORMAL]->texBuffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}