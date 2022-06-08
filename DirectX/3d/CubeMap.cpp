#include "CubeMap.h"
#include "Camera.h"
#include <DirectXTex.h>
#include <string>
#include "SafeDelete.h"

using namespace Microsoft::WRL;
using namespace DirectX;

ID3D12Device* CubeMap::device = nullptr;
ID3D12GraphicsCommandList* CubeMap::cmdList = nullptr;
std::unique_ptr<GraphicsPipelineManager> CubeMap::pipeline = nullptr;
Camera* CubeMap::camera = nullptr;

void CubeMap::CreateGraphicsPipeline()
{
	////頂点レイアウト配列の宣言と設定
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	pipeline = GraphicsPipelineManager::Create("CubeBox",
		GraphicsPipelineManager::OBJECT_KINDS::CUBE_BOX, inputLayout, _countof(inputLayout));
}

void CubeMap::StaticInitialize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!CubeMap::device);
	assert(device);
	CubeMap::device = device;

	CreateGraphicsPipeline();
}

std::unique_ptr<CubeMap> CubeMap::Create(ID3D12GraphicsCommandList* cmdList)
{
	//インスタンスを生成
	CubeMap* instance = new CubeMap();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->texture = Texture::Create("Resources/CubeMap/cubemap.dds", cmdList);

	//初期化
	instance->Initialize();

	return std::unique_ptr<CubeMap>(instance);
}

void CubeMap::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(CubeMap::cmdList == nullptr);

	CubeMap::cmdList = cmdList;

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipeline->pipelineState.Get());

	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipeline->rootSignature.Get());

	//プリミティブ形状の設定コマンド
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CubeMap::PostDraw()
{
	// コマンドリストを解除
	CubeMap::cmdList = nullptr;
}

void CubeMap::Finalize()
{
	pipeline.reset();
}

void CubeMap::Initialize()
{
	HRESULT result = S_FALSE;
	//頂点の長さ
	float edge = 1.0f;
	const int vertNum = 24;
	Vertex vertices[vertNum] = {
		//face1
		{{ -edge, -edge, -edge }, { -1.0f, -1.0f, -1.0f}}, // 左下 / 頂点1
		{{ -edge,  edge, -edge }, { -1.0f,  1.0f, -1.0f}}, // 左上 / 頂点2
		{{  edge, -edge, -edge }, { -1.0f, -1.0f,  1.0f}}, // 右下 / 頂点3
		{{  edge,  edge, -edge }, { -1.0f,  1.0f,  1.0f}}, // 右上 / 頂点4
		//face0
		{{  edge, -edge,  edge }, {  1.0f, -1.0f,  1.0f}}, // 左下 / 頂点5
		{{  edge,  edge,  edge }, {  1.0f,  1.0f,  1.0f}}, // 左上 / 頂点6
		{{ -edge, -edge,  edge }, {  1.0f, -1.0f, -1.0f}}, // 右下 / 頂点7
		{{ -edge,  edge,  edge }, {  1.0f,  1.0f, -1.0f}}, // 右上 / 頂点8
		//face4
		{{  edge, -edge, -edge }, { -1.0f, -1.0f,  1.0f}}, // 左下 / 頂点3
		{{  edge,  edge, -edge }, { -1.0f,  1.0f,  1.0f}}, // 左上 / 頂点4
		{{  edge, -edge,  edge }, {  1.0f, -1.0f,  1.0f}}, // 右下 / 頂点5
		{{  edge,  edge,  edge }, {  1.0f,  1.0f,  1.0f}}, // 右上 / 頂点6
		//face5
		{{ -edge, -edge,  edge }, {  1.0f, -1.0f, -1.0f}}, // 左下 / 頂点3
		{{ -edge,  edge,  edge }, {  1.0f,  1.0f, -1.0f}}, // 左上 / 頂点4
		{{ -edge, -edge, -edge }, { -1.0f, -1.0f, -1.0f}}, // 右下 / 頂点1
		{{ -edge,  edge, -edge }, { -1.0f,  1.0f, -1.0f}}, // 右上 / 頂点2
		//face2
		{{  edge,  edge, -edge }, { -1.0f,  1.0f,  1.0f}}, // 左下 / 頂点4
		{{ -edge,  edge, -edge }, { -1.0f,  1.0f, -1.0f}}, // 左上 / 頂点2
		{{  edge,  edge,  edge }, {  1.0f,  1.0f,  1.0f}}, // 右下 / 頂点6
		{{ -edge,  edge,  edge }, {  1.0f,  1.0f, -1.0f}}, // 右上 / 頂点8
		//face3
		{{ -edge, -edge, -edge }, { -1.0f, -1.0f, -1.0f}}, // 左下 / 頂点1
		{{  edge, -edge, -edge }, { -1.0f, -1.0f,  1.0f}}, // 左上 / 頂点3
		{{ -edge, -edge,  edge }, {  1.0f, -1.0f, -1.0f}}, // 右下 / 頂点7
		{{  edge, -edge,  edge }, {  1.0f, -1.0f,  1.0f}}, // 右上 / 頂点5
	};

	//頂点バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(Vertex) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの生成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(Vertex) * vertNum;
	vbView.StrideInBytes = sizeof(Vertex);

	unsigned short indices[indexNum] = {
		0 ,2 ,1 ,2 ,3 ,1 ,
		4 ,6 ,5 ,6 ,7 ,5 ,
		8 ,10,9 ,10,11,9 ,
		12,14,13,14,15,13,
		16,18,17,18,19,17,
		20,22,21,22,23,21
	};

	//インデックスバッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(unsigned short) * indexNum), // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	assert(SUCCEEDED(result));

	//インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	memcpy(indexMap, indices, sizeof(indices));
	indexBuff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(unsigned short) * indexNum;

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));
}

CubeMap::~CubeMap()
{
	vertBuff.Reset();
	indexBuff.Reset();
	texConstBuffer.Reset();
	constBuff.Reset();
	texture.reset();
}

void CubeMap::Update()
{
	//ワールド行列変換
	XMMATRIX matWorld = XMMatrixIdentity();
	XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matWorld *= matScale;

	XMMATRIX matRot;//角度
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.y));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.z));
	matWorld *= matRot;

	XMMATRIX matTrans;//平行方向
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);
	matWorld *= matTrans;

	const XMMATRIX& matViewProjection = camera->GetView() * camera->GetProjection();
	const XMFLOAT3& cameraPos = camera->GetEye();

	//定数バッファにデータを転送
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);//マッピング
	assert(SUCCEEDED(result));
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constBuff->Unmap(0, nullptr);
}

void CubeMap::Draw()
{
	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//頂点バッファの設定
	cmdList->IASetIndexBuffer(&ibView);

	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//テクスチャバッファ転送
	TransferTextureBubber(cmdList, 1);

	//描画コマンド
	cmdList->DrawIndexedInstanced(indexNum, 1, 0, 0, 0);
}

void CubeMap::TransferTextureBubber(ID3D12GraphicsCommandList* cmdList,UINT RootParameterIndex)
{
	cmdList->SetGraphicsRootDescriptorTable(RootParameterIndex, texture->descriptor->gpu);
}