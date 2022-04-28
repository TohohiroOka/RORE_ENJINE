#include "Pmx.h"
#include "Camera.h"

using namespace Microsoft::WRL;
using namespace DirectX;

ID3D12Device* Pmx::device = nullptr;
ID3D12GraphicsCommandList* Pmx::cmdList = nullptr;
Camera* Pmx::camera = nullptr;
LightGroup* Pmx::lightGroup = nullptr;
std::unique_ptr<GraphicsPipelineManager> Pmx::pipeline;

void Pmx::StaticInitialize(ID3D12Device* device)
{
	// 初期化チェック
	assert(!Pmx::device);

	// nullptrチェック
	assert(device);

	Pmx::device = device;

	PmxModel::StaticInitialize(device);

	// グラフィックパイプラインの生成
	CreateGraphicsPipeline();
}

void Pmx::CreateGraphicsPipeline()
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
	};

	pipeline = GraphicsPipelineManager::Create("Object3d",
		GraphicsPipelineManager::OBJECT_KINDS::OBJ, inputLayout, _countof(inputLayout));
}

bool Pmx::Initialize()
{
	// nullptrチェック
	assert(device);

	name = typeid(*this).name();

	HRESULT result;
	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	return true;
}

std::unique_ptr<Pmx> Pmx::Create(PmxModel* model)
{
	// 3Dオブジェクトのインスタンスを生成
	Pmx* instance = new Pmx();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
	}

	if (model) {
		instance->SetModel(model);
	}

	return std::unique_ptr<Pmx>(instance);
}

void Pmx::Update()
{
	assert(Pmx::camera);

	HRESULT result;

	UpdateWorldMatrix();

	const XMMATRIX& matViewProjection = camera->GetView() * camera->GetProjection();
	const XMFLOAT3& cameraPos = camera->GetEye();

	// 定数バッファへデータ転送
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
}

void Pmx::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Pmx::cmdList == nullptr);

	// コマンドリストをセット
	Pmx::cmdList = cmdList;

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipeline->pipelineState.Get());

	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipeline->rootSignature.Get());

	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Pmx::PostDraw()
{
	// コマンドリストを解除
	Pmx::cmdList = nullptr;
}

void Pmx::Draw()
{
	// nullptrチェック
	assert(device);
	assert(Pmx::cmdList);

	// モデルの割り当てがなければ描画しない
	if (model == nullptr) {
		return;
	}

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// ライトの描画
	lightGroup->Draw(cmdList, 3);

	// モデル描画
	model->Draw(cmdList);
}

void Pmx::UpdateWorldMatrix()
{
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映

	// 親オブジェクトがあれば
	if (parent != nullptr) {
		// 親オブジェクトのワールド行列を掛ける
		matWorld *= parent->matWorld;
	}
}

Pmx::~Pmx()
{
	constBuffB0.Reset();
}
