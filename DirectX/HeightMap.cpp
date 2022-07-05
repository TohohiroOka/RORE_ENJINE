#include "HeightMap.h"
#include "Camera.h"
#include <DirectXTex.h>
#include <string>
#include "SafeDelete.h"

using namespace Microsoft::WRL;
using namespace DirectX;

ID3D12Device* HeightMap::device = nullptr;
ID3D12GraphicsCommandList* HeightMap::cmdList = nullptr;
std::unique_ptr<GraphicsPipelineManager> HeightMap::pipeline = nullptr;
Camera* HeightMap::camera = nullptr;
LightGroup* HeightMap::lightGroup = nullptr;
const std::string HeightMap::baseDirectory = "Resources/HeightMap/";

void HeightMap::CreateGraphicsPipeline()
{
	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xyz座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	pipeline = GraphicsPipelineManager::Create("HeightMap",
		GraphicsPipelineManager::OBJECT_KINDS::HEIGHT_MAP, inputLayout, _countof(inputLayout));
}

void HeightMap::StaticInitialize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!HeightMap::device);
	assert(device);
	HeightMap::device = device;

	CreateGraphicsPipeline();
}

std::unique_ptr<HeightMap> HeightMap::Create(const std::string heightmapFilename, const std::string filename)
{
	//インスタンスを生成
	HeightMap* instance = new HeightMap();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->HeightMapLoad(heightmapFilename);

	instance->LoadTexture(filename);

	//初期化
	instance->Initialize();

	return std::unique_ptr<HeightMap>(instance);
}

void HeightMap::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(HeightMap::cmdList == nullptr);

	HeightMap::cmdList = cmdList;

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipeline->pipelineState.Get());

	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipeline->rootSignature.Get());

	//プリミティブ形状の設定コマンド
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void HeightMap::PostDraw()
{
	// コマンドリストを解除
	HeightMap::cmdList = nullptr;
}

void HeightMap::Finalize()
{
	pipeline.reset();
}

bool HeightMap::HeightMapLoad(const std::string filename)
{
	//名前結合
	std::string fname = baseDirectory + filename;

	//////WICテクスチャのロード
	//DirectX::TexMetadata metadata{};
	//DirectX::ScratchImage scratchImage{};

	////ユニコードに変換
	//wchar_t wfilePath[128];
	//int iBufferSize = MultiByteToWideChar(CP_ACP, 0,
	//	fname.c_str(), -1, wfilePath, _countof(wfilePath));

	//result = LoadFromWICFile(
	//	wfilePath,
	//	DirectX::WIC_FLAGS_NONE,
	//	&metadata, scratchImage);
	//assert(SUCCEEDED(result));

	//const DirectX::Image* img = scratchImage.GetImage(0, 0, 0);

	//height mapを開く
	FILE* filePtr;
	int error = fopen_s(&filePtr, fname.c_str(), "rb");
	if (error != 0)
	{
		return false;
	}

	//ファイルヘッダーの読み込み
	BITMAPFILEHEADER bitmapFileHeader;
	size_t count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//ビットマップ情報ヘッダーの読み込み
	BITMAPINFOHEADER bitmapInfoHeader;
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//地形の縦横幅を保存
	hmInfo.terrainWidth = bitmapInfoHeader.biWidth;
	hmInfo.terrainHeight = bitmapInfoHeader.biHeight;

	//ビットマップ画像データのサイズを計算
	int imageSize = hmInfo.terrainWidth * hmInfo.terrainHeight * 3;

	//ビットマップ画像データにメモリを割り当て
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	//ビットマップデータの先頭に移動
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//ビットマップ画像データの読み込み
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	//ファイルを閉じる
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//保存用配列をマップの大きさに変更
	size_t resize = hmInfo.terrainWidth * hmInfo.terrainHeight;
	hmInfo.heightMap.resize(resize);

	//赤要素のみ使用するための変数
	int k = 0;

	//地形をちょうどよくするために割る値
	float heightFactor = 10.0f;

	// 画像データの読み込み
	for (int j = 0; j < hmInfo.terrainHeight; j++)
	{
		for (int i = 0; i < hmInfo.terrainWidth; i++)
		{
			int height = static_cast<int>(bitmapImage[k]);

			int index = (hmInfo.terrainHeight * j) + i;

			hmInfo.heightMap[index].x = (float)i;
			hmInfo.heightMap[index].y = (float)height;
			hmInfo.heightMap[index].z = (float)j;

			k += 3;
		}
	}

	//ビットマップ画像データの解放
	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

void HeightMap::LoadTexture(const std::string filename)
{
	// テクスチャなし
	std::string filepath;
	if (filename == "null") {
		filepath = "Resources/SubTexture/white1x1.png";
	}
	else
	{
		filepath = baseDirectory + filename;
	}

	texture = Texture::Create(filepath);
}

void HeightMap::Initialize()
{
	HRESULT result = S_FALSE;

	int windthSize = hmInfo.terrainWidth - 1;
	int heightSize = hmInfo.terrainHeight - 1;

	int surfaceNum = windthSize * heightSize;
	vertNum = surfaceNum * 4;
	indexNum = surfaceNum * 6;

	std::vector<Vertex> vertices;
	vertices.resize(vertNum);
	if (vertices.size() == 0)
	{
		assert(0);
	}
	std::vector<unsigned long> indices;
	indices.resize(indexNum);

	//unsigned long basicsIndices[6] = { 0 ,2 ,1 ,2 ,3 ,1 };
	unsigned long basicsIndices[6] = { 1,3,2,1,2,0 };
	//挿入インデックス番号
	unsigned long index = 0;

	for (int j = 0; j < heightSize; ++j)
	{
		for (int i = 0; i < windthSize; ++i)
		{
			int index1 = (hmInfo.terrainHeight * j) + i;// Bottom left.
			int index2 = (hmInfo.terrainHeight * j) + (i + 1);// Bottom right.
			int index3 = (hmInfo.terrainHeight * (j + 1)) + i;// Upper left.
			int index4 = (hmInfo.terrainHeight * (j + 1)) + (i + 1);// Upper right.

			// Bottom left.
			vertices[index].pos.x = hmInfo.heightMap[index1].x;
			vertices[index].pos.y = hmInfo.heightMap[index1].y;
			vertices[index].pos.z = hmInfo.heightMap[index1].z;
			index++;

			// Upper left.
			vertices[index].pos.x = hmInfo.heightMap[index3].x;
			vertices[index].pos.y = hmInfo.heightMap[index3].y;
			vertices[index].pos.z = hmInfo.heightMap[index3].z;
			index++;

			// Bottom right.
			vertices[index].pos.x = hmInfo.heightMap[index2].x;
			vertices[index].pos.y = hmInfo.heightMap[index2].y;
			vertices[index].pos.z = hmInfo.heightMap[index2].z;
			index++;

			// Upper right.
			vertices[index].pos.x = hmInfo.heightMap[index4].x;
			vertices[index].pos.y = hmInfo.heightMap[index4].y;
			vertices[index].pos.z = hmInfo.heightMap[index4].z;
			index++;
		}
	}

	for (int i = 0; i < surfaceNum; i++)
	{
		int num = 0;
		index = i * 6;
		indices[index + num] = basicsIndices[num] + index;
		num++;
		indices[index + num] = basicsIndices[num] + index;
		num++;
		indices[index + num] = basicsIndices[num] + index;
		num++;
		indices[index + num] = basicsIndices[num] + index;
		num++;
		indices[index + num] = basicsIndices[num] + index;
		num++;
		indices[index + num] = basicsIndices[num] + index;
	}

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
	std::copy(vertices.begin(), vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの生成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(Vertex) * vertNum;
	vbView.StrideInBytes = sizeof(Vertex);

	//インデックスバッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(unsigned long) * indexNum), // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	assert(SUCCEEDED(result));

	//インデックスバッファへのデータ転送
	unsigned long* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	std::copy(indices.begin(), indices.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R32_UINT;
	ibView.SizeInBytes = sizeof(unsigned long) * indexNum;

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

HeightMap::~HeightMap()
{
	vertBuff.Reset();
	indexBuff.Reset();
	constBuff.Reset();
	texture.reset();
}

void HeightMap::Update()
{
	//ワールド行列変換
	XMMATRIX matWorld = XMMatrixIdentity();
	XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matWorld *= matScale;

	XMMATRIX matRot;//角度
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(0));
	matRot *= XMMatrixRotationX(XMConvertToRadians(0));
	matRot *= XMMatrixRotationY(XMConvertToRadians(0));
	matWorld *= matRot;

	XMMATRIX matTrans;//平行方向
	matTrans = XMMatrixTranslation(0, 0, 0);
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

void HeightMap::Draw()
{
	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//頂点バッファの設定
	cmdList->IASetIndexBuffer(&ibView);

	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	// ライトの描画
	lightGroup->Draw(cmdList, 2);

	//テクスチャバッファ転送
	cmdList->SetGraphicsRootDescriptorTable(1, texture->descriptor->gpu);

	//描画コマンド
	cmdList->DrawIndexedInstanced(indexNum, 1, 0, 0, 0);
}