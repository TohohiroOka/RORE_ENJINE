#include "PrimitiveObject3D.h"
#include "Camera.h"
#include "SafeDelete.h"

#include <vector>

using namespace DirectX;

GraphicsPipelineManager::GRAPHICS_PIPELINE PrimitiveObject3D::pipeline;

PrimitiveObject3D::~PrimitiveObject3D()
{
	//バッファを解放
	vertBuff.Reset();
	indexBuff.Reset();
}

void PrimitiveObject3D::Initialize()
{
	HRESULT result = S_FALSE;

	//頂点配列の大きさ
	size_t vertSize = vertices.size();

	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	const UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * vertSize);

	//頂点バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	if (FAILED(result)) {
		assert(0);
	}

	// 頂点バッファへのデータ転送
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	//頂点バッファビューの生成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	//使うインデックスの作成
	indices.resize(vertSize);
	for (int i = 0; i < static_cast<int>(vertSize); i++)
	{
		indices[i] = i;
	}

	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	const UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	//インデックスバッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB), // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	if (FAILED(result)) {
		assert(0);
	}

	//インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	std::copy(indices.begin(), indices.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	InterfaceObject3d::Initialize();
}

void PrimitiveObject3D::Update()
{
	assert(camera);

	const XMMATRIX& matViewProjection = camera->GetView() * camera->GetProjection();
	const XMFLOAT3& cameraPos = camera->GetEye();

	//定数バッファにデータを転送
	ConstBufferDataB0* constMap = nullptr;
	HRESULT result = constBuffB0->Map(0, nullptr, (void**)&constMap);//マッピング
	assert(SUCCEEDED(result));
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constMap->isSkinning = isSkinning;
	constMap->isBloom = isBloom;
	constMap->isToon = isToon;
	constMap->isOutline = isOutline;
	constBuffB0->Unmap(0, nullptr);

	ConstBufferDataB1* constMapB1 = nullptr;
	result = constBuffB1->Map(0, nullptr, (void**)&constMapB1);//マッピング
	assert(SUCCEEDED(result));
	constMapB1->baseColor = constBufferB1Num.baseColor;
	constMapB1->ambient = constBufferB1Num.ambient;
	constMapB1->diffuse = constBufferB1Num.diffuse;
	constMapB1->metalness = constBufferB1Num.metalness;
	constMapB1->specular = constBufferB1Num.specular;
	constMapB1->roughness = constBufferB1Num.roughness;
	constMapB1->alpha = constBufferB1Num.alpha;;
	constBuffB1->Unmap(0, nullptr);
}

void PrimitiveObject3D::PreDraw()
{
	// パイプラインステートの設定
	cmdList->SetPipelineState(pipeline.pipelineState.Get());

	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipeline.rootSignature.Get());

	//プリミティブ形状の設定コマンド
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void PrimitiveObject3D::Draw()
{
	//インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView);

	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	//描画コマンド
	cmdList->DrawInstanced(static_cast<UINT>(indices.size()), 1, 0, 0);
}