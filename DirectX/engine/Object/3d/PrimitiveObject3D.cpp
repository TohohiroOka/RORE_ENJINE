#include "PrimitiveObject3D.h"
#include "Camera/Camera.h"
#include "SafeDelete.h"

#include <vector>

using namespace DirectX;

std::vector<GraphicsPipelineManager::DrawSet> PrimitiveObject3D::pipeline;

std::unique_ptr<PrimitiveObject3D> PrimitiveObject3D::Create()
{
	//インスタンスを生成
	PrimitiveObject3D* instance = new PrimitiveObject3D();
	if (instance == nullptr) {
		return nullptr;
	}

	//初期化
	instance->Initialize();

	//ワールド行列生成
	instance->UpdateWorldMatrix();

	return std::unique_ptr<PrimitiveObject3D>(instance);
}

PrimitiveObject3D::~PrimitiveObject3D()
{
	DeleteCollider();
}

void PrimitiveObject3D::Initialize()
{
	HRESULT result = S_FALSE;

	if (vertices.size() != 0) {
		VertexInit();
	}
	//定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(CONST_BUFFER_DATA) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));
	if (FAILED(result)) { assert(0); }
}

void PrimitiveObject3D::Update()
{
	assert(camera);

	const XMMATRIX& matViewProjection = camera->GetView() * camera->GetProjection();

	//定数バッファにデータを転送
	CONST_BUFFER_DATA* constMap = nullptr;
	HRESULT result = constBuffB0->Map(0, nullptr, (void**)&constMap);//マッピング
	if (SUCCEEDED(result)) {
		constMap->color = baseColor;
		constMap->matWorld = matWorld;
		if (camera != nullptr)
		{
			constMap->viewproj = matViewProjection;
		} else {
			constMap->viewproj = matViewProjection;
		}
		constBuffB0->Unmap(0, nullptr);
	}
}

void PrimitiveObject3D::Draw(const DrawMode _drawMode)
{
	Update();

	int modeNum = int(_drawMode);

	ObjectBase::Draw(pipeline[modeNum]);

	//インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView);

	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	//描画コマンド
	cmdList->DrawIndexedInstanced(static_cast<UINT>(indices.size()), 1, 0, 0, 0);
}

void PrimitiveObject3D::VertexInit()
{
	HRESULT result = S_FALSE;

	//頂点配列の大きさ
	size_t vertSize = vertices.size();

	vertBuff.Reset();
	indexBuff.Reset();

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
	if (FAILED(result)) { assert(0); }

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
	const UINT sizeIB = static_cast<UINT>(sizeof(unsigned long) * indices.size());

	//インデックスバッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB), // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	if (FAILED(result)) { assert(0); }

	//インデックスバッファへのデータ転送
	unsigned long* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}
	
	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R32_UINT;
	ibView.SizeInBytes = sizeIB;
}
