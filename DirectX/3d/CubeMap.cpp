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
ComPtr<ID3D12Resource> CubeMap::texBuffer = nullptr;
ComPtr<ID3D12DescriptorHeap> CubeMap::descHeap = nullptr;
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

void CubeMap::StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	// 再初期化チェック
	assert(!CubeMap::device);
	assert(device);
	CubeMap::device = device;

	// 再初期化チェック
	assert(!CubeMap::cmdList);
	assert(cmdList);
	CubeMap::cmdList = cmdList;

	//デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 6;
	HRESULT result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));
	assert(SUCCEEDED(result));

	CreateGraphicsPipeline();
}

std::unique_ptr<CubeMap> CubeMap::Create()
{
	//インスタンスを生成
	CubeMap* instance = new CubeMap();
	if (instance == nullptr) {
		return nullptr;
	}

	//テクスチャ読み込み
	instance->LoadCubeTexture();

	//初期化
	instance->Initialize();

	texBuffer->SetName(L"CubeTex");
	descHeap->SetName(L"Cube_DescHeap");

	return std::unique_ptr<CubeMap>(instance);
}

void CubeMap::SetCubeMap(ID3D12GraphicsCommandList* cmdList)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		texBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);
}

void CubeMap::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	CubeMap::cmdList = cmdList;

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipeline->pipelineState.Get());

	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipeline->rootSignature.Get());

	//プリミティブ形状の設定コマンド
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//デスクリプタヒープをセット
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void CubeMap::PostDraw()
{
	// コマンドリストを解除
	CubeMap::cmdList = nullptr;
}

void CubeMap::Finalize()
{
	descHeap.Reset();
	texBuffer.Reset();
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

void CubeMap::LoadCubeTexture()
{
	HRESULT result = S_FALSE;

	{
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;

		result = DirectX::LoadFromDDSFile(L"Resources/CubeMap/cubemap.dds", DDS_FLAGS_NONE, &metadata, image);
		assert(SUCCEEDED(result));

		result = CreateTexture(device, metadata, &texBuffer);
		assert(SUCCEEDED(result));

		result = PrepareUpload(device, image.GetImages(), image.GetImageCount(), metadata, subresources);
		assert(SUCCEEDED(result));

		//サイズ
		UINT totalBytes = (UINT)GetRequiredIntermediateSize(texBuffer.Get(), 0, UINT(subresources.size()));

		//テクスチャ転送用バッファ生成
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(totalBytes),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&texConstBuffer));
		assert(SUCCEEDED(result));

		UpdateSubresources(
			cmdList,
			texBuffer.Get(), texConstBuffer.Get(),
			0, 0, (UINT)subresources.size(), subresources.data());

		SetCubeMap(cmdList);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = (UINT)metadata.mipLevels;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		device->CreateShaderResourceView(texBuffer.Get(), &srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), 0,
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
	}

}

CubeMap::~CubeMap()
{
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

	//シェーダーリソースビューをセット
	auto heapStart = descHeap->GetGPUDescriptorHandleForHeapStart();

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
	cmdList->SetGraphicsRootDescriptorTable(RootParameterIndex,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
}