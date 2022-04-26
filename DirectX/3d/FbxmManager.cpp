#include "FbxmManager.h"
#include "Camera.h"
#include "BaseCollider.h"
#include "SafeDelete.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

ID3D12Device* FbxmManager::device = nullptr;
Camera* FbxmManager::camera = nullptr;
LightGroup* FbxmManager::lightGroup = nullptr;
ID3D12GraphicsCommandList* FbxmManager::cmdList = nullptr;
std::unique_ptr<GraphicsPipelineManager> FbxmManager::pipeline = nullptr;

FbxmManager::~FbxmManager()
{
	constBuffB0.Reset();
}

void FbxmManager::CreateGraphicsPipeline()
{
	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xyz座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 法線ベクトル
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv座標
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//ボーン番号
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//ボーン影響度
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	pipeline = GraphicsPipelineManager::Create("Fbx",
		GraphicsPipelineManager::OBJECT_KINDS::FBX, inputLayout, _countof(inputLayout));
}

void FbxmManager::StaticInitialize(ID3D12Device* device)
{
	HRESULT result = S_FALSE;

	// 再初期化チェック
	assert(!FbxmManager::device);

	// nullptrチェック
	assert(device);

	FbxmManager::device = device;

	CreateGraphicsPipeline();

	FbxModel::StaticInitialize(device);
}

void FbxmManager::Initialize()
{
	HRESULT result = S_FALSE;
	// 定数バッファ1の生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));
	assert(SUCCEEDED(result));
}

std::unique_ptr<FbxmManager> FbxmManager::Create(FbxModel* model)
{
	// 3Dオブジェクトのインスタンスを生成
	FbxmManager* instance = new FbxmManager();

	//初期化
	instance->Initialize();

	//モデルが指定されていればセットする
	if (model) {
		instance->SetModel(model);
	}

	return std::unique_ptr<FbxmManager>(instance);
}

void FbxmManager::Update()
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映

	const XMMATRIX& matViewProjection = camera->GetView() * camera->GetProjection();
	const XMFLOAT3& cameraPos = camera->GetEye();

	// 定数バッファ1へデータ転送
	ConstBufferDataB0* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constMap->isBloom = isBloom;
	constMap->isToon = isToon;
	constMap->isOutline = isOutline;
	constBuffB0->Unmap(0, nullptr);

	model->Update();
}

void FbxmManager::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	FbxmManager::cmdList = cmdList;

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipeline->pipelineState.Get());

	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipeline->rootSignature.Get());

	//プリミティブ形状の設定コマンド
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	FbxModel::PreDraw(cmdList);
}

void FbxmManager::PostDraw()
{
	// コマンドリストを解除
	FbxmManager::cmdList = nullptr;
}

void FbxmManager::Draw()
{
	// nullptrチェック
	assert(device);
	assert(FbxmManager::cmdList);

	// モデルの割り当てがなければ描画しない
	if (model == nullptr) {
		return;
	}

	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// ライトの描画
	lightGroup->Draw(cmdList, 3);

	// モデル描画
	model->Draw(cmdList);
}

void FbxmManager::Finalize()
{
	FbxModel::Finalize();
	pipeline.reset();
}