#include "BasePostEffect.h"
#include "WindowApp.h"

using namespace DirectX;

const float BasePostEffect::clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
std::vector<GraphicsPipelineManager::DrawSet> BasePostEffect::pipeline;

void BasePostEffect::Initialize()
{
	HRESULT result;

	//頂点バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VERTEX) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	if (FAILED(result)) { assert(0); }

	// 頂点データ
	VERTEX vertices
		[vertNum] = {
		{{ -1,-1,0.0f }, {0.0f,1.0f}}, // 左下
		{{ -1,+1,0.0f }, {0.0f,0.0f}}, // 左上
		{{ +1,-1,0.0f }, {1.0f,1.0f}}, // 右下
		{{ +1,+1,0.0f }, {1.0f,0.0f}}, // 右上
	};

	// 頂点バッファへのデータ転送
	VERTEX* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VERTEX) * 4;
	vbView.StrideInBytes = sizeof(VERTEX);

	std::array<UINT, 2> texSize = texSize = { WindowApp::GetWindowWidth(),WindowApp::GetWindowHeight() };

	//テクスチャバッファ生成用変数
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		texSize[0], texSize[1],
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	// テクスチャ用バッファの生成
	texture = std::make_unique<Texture>();

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // テクスチャ用指定
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor),
		IID_PPV_ARGS(&texture->texBuffer));
	if (FAILED(result)) { assert(0); }

	//テクスチャを赤クリア
	{
		//画素数
		const UINT pixel_count = texSize[0] * texSize[1];
		//画素一行分のデータサイズ
		const UINT row_pitch = sizeof(UINT) * texSize[0];
		//画素全体のデータサイズ
		const UINT depth_pitch = row_pitch * texSize[1];
		//画素イメージ
		UINT* img = new UINT[pixel_count];
		for (UINT j = 0; j < pixel_count; j++) { img[j] = 0xff0000ff; }

		// テクスチャバッファにデータ転送
		result = texture->texBuffer->WriteToSubresource(0, nullptr,
			img, row_pitch, depth_pitch);
		if (FAILED(result)) { assert(0); }
		delete[] img;
	}

	//SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//デスクリプタヒープにSRV生成
	texture->descriptor = std::make_unique<DescriptorHeapManager>();
	texture->descriptor->CreateSRV(texture->texBuffer, srvDesc);

	//RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHescDesc{};
	rtvDescHescDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHescDesc.NumDescriptors = 1;

	//RTV用デスクリプタヒープを生成
	result = device->CreateDescriptorHeap(&rtvDescHescDesc, IID_PPV_ARGS(&descHeapRTV));
	if (FAILED(result)) { assert(0); }

	//デスクリプタヒープにRTV生成
	device->CreateRenderTargetView(texture->texBuffer.Get(), nullptr,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeapRTV->GetCPUDescriptorHandleForHeapStart(), 0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));
}

void BasePostEffect::CreateDepthBuffer()
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
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値書き込みに使用
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer));
	if (FAILED(result)) { assert(0); }

	//DSV用デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビュー
	dsvHeapDesc.NumDescriptors = 1;//深度ビューは1つ

	//デスクリプタヒープにDSV生成
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	if (FAILED(result)) { assert(0); }

	//深度バッファリソース設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(depthBuffer.Get(), &dsvDesc, descHeapDSV->GetCPUDescriptorHandleForHeapStart());
}

void BasePostEffect::CreateConstBuffer(const EffectTyep _type)
{
	type = _type;

	HRESULT result;

	// 定数バッファの生成
	if (type == EffectTyep::bloom) {
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(CONST_BUFFER_DATA_BLOOM) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuff));
		if (FAILED(result)) { assert(0); }
	}else if(type == EffectTyep::outline) {
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(CONST_BUFFER_DATA_OUTLINE) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuff));
		if (FAILED(result)) { assert(0); }
	}
}

void BasePostEffect::Draw(const Texture* _tex)
{
	HRESULT result;
	// 定数バッファへデータ転送
	if (type == EffectTyep::bloom) {
		CONST_BUFFER_DATA_BLOOM* constMap = nullptr;
		result = constBuff->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {
			constMap->strength = strength;
			constBuff->Unmap(0, nullptr);
		}
	} else if (type == EffectTyep::outline) {
		CONST_BUFFER_DATA_OUTLINE* constMap = nullptr;
		result = constBuff->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {
			constMap->outlineWidth = outlineWidth;
			constBuff->Unmap(0, nullptr);
		}
	}

	int modeNum = int(type);

	ObjectBase::Draw(pipeline[modeNum]);

	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &this->vbView);

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, this->constBuff->GetGPUVirtualAddress());

	//シェーダーリソースビュー
	cmdList->SetGraphicsRootDescriptorTable(pipeline[modeNum].constBuffNum, _tex->descriptor->gpu);

	// 描画コマンド
	cmdList->DrawInstanced(4, 1, 0, 0);
}

BasePostEffect::BasePostEffect()
	:Sprite()
{
	//ブルームの強さ
	strength = 0.001f;
	//アウトラインの太さ初期化
	outlineWidth = 0.005f;
}

BasePostEffect::~BasePostEffect()
{
}

std::unique_ptr<BasePostEffect> BasePostEffect::Create(const EffectTyep _type)
{
	// Spriteのインスタンスを生成
	BasePostEffect* instance = new BasePostEffect();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();
	//定数バッファ
	instance->CreateConstBuffer(_type);
	//深度バッファ生成
	instance->CreateDepthBuffer();

	//ユニークポインタを返す
	return std::unique_ptr<BasePostEffect>(instance);
}

void BasePostEffect::PreDrawScene()
{
	//リソースバリアを変更
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(texture->texBuffer.Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET));

	//レンダーターゲットビュー用デスクリプタヒープのハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHs = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeapRTV->GetCPUDescriptorHandleForHeapStart(), 0,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	//深度ステンシルビュー用デスクリプタヒープのハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	//レンダーターゲットをセット
	cmdList->OMSetRenderTargets(1, &rtvHs, false, &dsvH);

	//ビューポート設定
	CD3DX12_VIEWPORT viewports =
		CD3DX12_VIEWPORT(0.0f, 0.0f, FLOAT(WindowApp::GetWindowWidth()), FLOAT(WindowApp::GetWindowHeight()));
	CD3DX12_RECT scissorRects =
		CD3DX12_RECT(0, 0, LONG(WindowApp::GetWindowWidth()), LONG(WindowApp::GetWindowHeight()));

	cmdList->RSSetViewports(1, &viewports);
	//シザリング矩形設定
	cmdList->RSSetScissorRects(1, &scissorRects);

	//全画面クリア
	cmdList->ClearRenderTargetView(rtvHs, clearColor, 0, nullptr);

	//深度バッファのクリア
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void BasePostEffect::PostDrawScene()
{
	//リソースバリアを変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texture->texBuffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}