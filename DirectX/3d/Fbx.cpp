#include "Fbx.h"
#include "Camera.h"
#include "BaseCollider.h"
#include "SafeDelete.h"

#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

FbxManager* Fbx::fbxManager = nullptr;
ID3D12Device* Fbx::device = nullptr;
Camera* Fbx::camera = nullptr;
ID3D12GraphicsCommandList* Fbx::cmdList = nullptr;
std::vector<Fbx::Data> Fbx::data;
std::unique_ptr<GraphicsPipelineManager> Fbx::pipeline = nullptr;
ComPtr<ID3D12DescriptorHeap> Fbx::descHeap;
ComPtr<ID3D12Resource> Fbx::texBuffer[textureNum];
FbxTime Fbx::frameTime;
int Fbx::vecSize = 0;
const std::string Fbx::subTexture = "Resources/subTexture/white1x1.png";
const string Fbx::directoryPath = "Resources/Fbx/";
std::string Fbx::fileName;

Fbx::~Fbx()
{
	int size = (UINT)data.size();
	for (int i = 0; i < size; i++)
	{
		data[i].fbxUpdate.fbxScene->Destroy();
	}

	constBuffTransform.Reset();
	constBuffSkin.Reset();
	vertBuff.Reset();
	indexBuff.Reset();

	data.clear();
}

void Fbx::CreateGraphicsPipeline()
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

void Fbx::LoadMaterial(int dataNumber, FbxNode* fbxNode) {
	HRESULT result = S_FALSE;

	//テクスチャの有無確認
	bool textureLoaded = false;

	//マテリアルの個数確認
	const int materialCount = fbxNode->GetMaterialCount();
	if (materialCount > 0)
	{
		//先頭のマテリアルを取得
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);

		if (material)
		{
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);

				//環境光係数
				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				data[dataNumber].material.ambient.x = (float)ambient.Get()[0];
				data[dataNumber].material.ambient.y = (float)ambient.Get()[1];
				data[dataNumber].material.ambient.z = (float)ambient.Get()[2];

				//環境光係数
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				data[dataNumber].material.diffuse.x = (float)diffuse.Get()[0];
				data[dataNumber].material.diffuse.y = (float)diffuse.Get()[1];
				data[dataNumber].material.diffuse.z = (float)diffuse.Get()[2];
			}

			//ディフューズテクスチャ
			const FbxProperty diffuseProperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (diffuseProperty.IsValid())
			{
				const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();

				if (texture)
				{
					const char* filePath = texture->GetFileName();

					//ファイルパスからファイル名取得
					string path_str(filePath);
					string name = ExtractFileName(path_str);

					//テクスチャ読み込み
					LoadTexture(dataNumber, directoryPath + fileName + '/' + name);
					textureLoaded = true;
				}
			}
		}

		//textureが無い場合白にする
		if (!textureLoaded)
		{
			LoadTexture(dataNumber, subTexture);
		}
	}
}

void Fbx::LoadTexture(int dataNumber, const std::string fname)
{
	HRESULT result;

	////WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImage{};

	//ユニコードに変換
	wchar_t wfilePath[128];
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0,
		fname.c_str(), -1, wfilePath, _countof(wfilePath));


	result = LoadFromWICFile(
		wfilePath,
		WIC_FLAGS_NONE,
		&metadata, scratchImage);

	const Image* img = scratchImage.GetImage(0, 0, 0);

	//テクスチャバッファの生成
	//リソース設定
	D3D12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	//テクスチャバッファ生成
	result = device->CreateCommittedResource(//GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texBuffer[dataNumber]));

	//テクスチャバッファにデータ転送
	result = texBuffer[dataNumber]->WriteToSubresource(
		0,
		nullptr,//全領域へコピー
		img->pixels,//元データアドレス
		(UINT)img->rowPitch,//１ラインサイズ
		(UINT)img->slicePitch//1枚サイズ
	);

	//テクスチャバッファ生成
	result = device->CreateCommittedResource(//GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texBuffer[dataNumber]));

	//テクスチャバッファにデータ転送
	result = texBuffer[dataNumber]->WriteToSubresource(
		0,
		nullptr,//全領域へコピー
		img->pixels,//元データアドレス
		(UINT)img->rowPitch,//１ラインサイズ
		(UINT)img->slicePitch//1枚サイズ
	);

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	//ヒープのtexnumber番目にシェーダーリソースビューを作成
	device->CreateShaderResourceView(
		texBuffer[dataNumber].Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), dataNumber,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
}

void Fbx::CollectMesh(int dataNumber, FbxNode* fbxNode)
{
	//メッシュ取得
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	//頂点情報読み取り
	CollectVertices(dataNumber, fbxMesh);

	//面を構成するデータの読み取り
	CollectMeshFaces(dataNumber, fbxMesh);

	//マテリアルの読み取り
	LoadMaterial(dataNumber, fbxNode);

	//スキニング情報の読み取り
	CollectSkin(dataNumber, fbxMesh);
}

void Fbx::CollectVertices(int dataNumber, FbxMesh* fbxMesh)
{
	//頂点座標データ数
	const int controlPointCount = fbxMesh->GetControlPointsCount();

	//Fbxメッシの頂点座標取得
	FbxVector4* vertex = fbxMesh->GetControlPoints();

	//配列のサイズ変更
	data[dataNumber].vertices.resize(controlPointCount);

	//頂点をコピー
	for (int i = 0; i < controlPointCount; i++)
	{
		data[dataNumber].vertices[i].pos.x = (float)vertex[i][0];
		data[dataNumber].vertices[i].pos.y = (float)vertex[i][1];
		data[dataNumber].vertices[i].pos.z = (float)vertex[i][2];
	}
}

void Fbx::CollectMeshFaces(int dataNumber, FbxMesh* fbxMesh)
{
	//面の数
	const int polygonCount = fbxMesh->GetPolygonCount();

	//UVデータの数
	const int textureUVCount = fbxMesh->GetTextureUVCount();

	//UV名リスト
	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);

	//面ごとの情報
	for (int i = 0; i < polygonCount; i++)
	{
		//面の頂点数取得
		const int polygonSize = fbxMesh->GetPolygonSize(i);
		assert(polygonSize <= 4);

		//1頂点ごとに読み込み
		for (int j = 0; j < polygonSize; j++)
		{
			int index = fbxMesh->GetPolygonVertex(i, j);
			assert(index >= 0);

			//取得した法線を一時格納する
			FbxVector4 normal;

			//法線読み込み
			if (fbxMesh->GetPolygonVertexNormal(i, j, normal))
			{
				data[dataNumber].vertices[index].normal.x = (float)normal[0];
				data[dataNumber].vertices[index].normal.y = (float)normal[1];
				data[dataNumber].vertices[index].normal.z = (float)normal[2];
			}

			//テクスチャuv読み込み
			if (textureUVCount > 0)
			{
				FbxVector2 uvs;
				bool LUnmappedUV;

				if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, LUnmappedUV))
				{
					data[dataNumber].vertices[index].uv.x = (float)uvs[0];
					data[dataNumber].vertices[index].uv.y = (float)uvs[1];
				}
			}

			//3頂点までのインデックス追加
			if (j < 3)
			{
				data[dataNumber].indices.push_back(index);
			}
			//4頂点目がある場合
			else
			{
				//仮格納
				int index2 = data[dataNumber].indices[data[dataNumber].indices.size() - 1];
				int index3 = index;
				int index0 = data[dataNumber].indices[data[dataNumber].indices.size() - 3];

				//本格納
				data[dataNumber].indices.push_back(index2);
				data[dataNumber].indices.push_back(index3);
				data[dataNumber].indices.push_back(index0);
			}
		}
	}
}

void Fbx::CollectSkin(int dataNumber, FbxMesh* fbxMesh)
{
	//スキニング情報
	FbxSkin* fbxSkin =
		static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));

	//スキニング情報が無ければ終了
	if (fbxSkin == nullptr)
	{
		return;
	}

	//ボーン配列の参照
	vector<Bone>& bones = data[dataNumber].bones;

	//ボーンの数
	int clusterCount = fbxSkin->GetClusterCount();

	//配列確保
	bones.reserve(clusterCount);

	//全てのボーンについて
	for (int i = 0; i < clusterCount; i++)
	{
		//Fbxボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		//ボーン自体のノードの名前を取得
		const char* boenName = fbxCluster->GetLink()->GetName();

		//ボーンを追加して参照を得る
		bones.emplace_back(Bone(boenName));
		Bone& bone = bones.back();

		//自作ボーンとFBXを紐づける
		bone.fbxCluster = fbxCluster;

		//FBXから初期姿勢行列を取得
		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		//XMMatrix型に変換
		XMMATRIX initialPos;
		ConvertMatrixFormFbx(&initialPos, fbxMat);

		//初期姿勢行列の逆行列の取得
		bone.invInitialPose = XMMatrixInverse(nullptr, initialPos);
	}

	//スキンウェイト
	struct WeightSet
	{
		UINT index;
		float weight;
	};

	//保存用配列
	vector<list<WeightSet>> weightLists(data[dataNumber].vertices.size());

	//全てのボーンについて
	for (int i = 0; i < clusterCount; i++)
	{
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		//ボーンに影響を受ける頂点の数
		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();

		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controlWeights = fbxCluster->GetControlPointWeights();

		//影響を受ける全頂点について
		for (int j = 0; j < controlPointIndicesCount; j++)
		{
			//頂点番号
			int vertIndex = controlPointIndices[j];

			//スキンウェイト
			float weight = (float)controlWeights[j];

			//その頂点の影響を受けるボーンリストに、ボーンをウェイトのペアを追加
			weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
		}
	}

	//頂点配列書き換え用の参照
	auto& vertices = data[dataNumber].vertices;

	//各頂点について処理
	for (int i = 0; i < vertices.size(); i++)
	{
		//頂点のウェイトから最も大きい4つを選択
		auto& weightList = weightLists[i];

		//大小企画用のラムダ式を指定して降順にソート
		weightList.sort(
			[](auto const& lhs, auto const& rhs)
			{
				//左の要素が大きければtrue,でなければfalse
				return lhs.weight > rhs.weight;
			});

		int weightArrayIndex = 0;

		//降順ソート済みのウェイトリストから
		for (auto& WeightSet : weightList)
		{
			//頂点データに書き込み
			vertices[i].boneIndex[weightArrayIndex] = WeightSet.index;
			vertices[i].boneWhight[weightArrayIndex] = WeightSet.weight;

			//4つに達したら終了
			if (++weightArrayIndex >= MAX_BONE_INDICES)
			{
				float weight = 0.0f;

				//2番目以降のウェイト計算
				for (int j = 1; j < MAX_BONE_INDICES; j++)
				{
					weight += vertices[i].boneWhight[j];
				}

				//合計で1.0f(100%)になるよう調整
				vertices[i].boneWhight[0] = 1.0f - weight;
				break;
			}
		}
	}
}

void Fbx::LoadNode(int dataNumber, FbxNode* fbxNode, Node* parent)
{
	Node virtualNode;

	virtualNode.name = fbxNode->GetName();

	//ノードのローカル情報
	FbxDouble3 rota = fbxNode->LclRotation;
	FbxDouble3 scaling = fbxNode->LclScaling;
	FbxDouble3 translation = fbxNode->LclTranslation;

	//形式変換して代入
	virtualNode.rotation = { (float)rota[0], (float)rota[1], (float)rota[2], 0.0f };
	virtualNode.scaling = { (float)scaling[0], (float)scaling[1], (float)scaling[2], 0.0f };
	virtualNode.translation = { (float)translation[0], (float)translation[1], (float)translation[2], 0.0f };

	//行列計算
	XMMATRIX matScaling, matRoration, matTranslation;
	matScaling = XMMatrixScalingFromVector(virtualNode.scaling);
	matRoration = XMMatrixRotationRollPitchYawFromVector(virtualNode.rotation);
	matTranslation = XMMatrixTranslationFromVector(virtualNode.translation);

	//変形行列計算
	virtualNode.transform = XMMatrixIdentity();
	virtualNode.transform *= matScaling;
	virtualNode.transform *= matRoration;
	virtualNode.transform *= matTranslation;

	//グローバル行列計算
	virtualNode.globalTransform = virtualNode.transform;
	if (parent)
	{
		//親を登録
		virtualNode.parent = parent;

		//親の変形を乗算
		virtualNode.globalTransform *= parent->globalTransform;
	}

	//メtッシュ情報の解析
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute)
	{
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			CollectMesh(dataNumber, fbxNode);
		}
	}

	//仮変数に入れたものをコンテナの一番後ろにセット
	data[dataNumber].nodes.push_back(virtualNode);

	//子ノードがあれば再帰する
	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		LoadNode(dataNumber, fbxNode->GetChild(i), &virtualNode);
	}
}

void Fbx::LoadAnimation(int dataNumber)
{
	//0番目のアニメーション取得
	FbxAnimStack* animstick = data[dataNumber].fbxUpdate.fbxScene->GetSrcObject<FbxAnimStack>(0);
	//アニメーションの名前取得
	const char* anumstackkname = animstick->GetName();
	//アニメーションの時間情報
	FbxTakeInfo* takeinfo = data[dataNumber].fbxUpdate.fbxScene->GetTakeInfo(anumstackkname);

	//開始時間
	data[dataNumber].fbxUpdate.startTime = takeinfo->mLocalTimeSpan.GetStart();

	//終了時間
	data[dataNumber].fbxUpdate.stopTime = takeinfo->mLocalTimeSpan.GetStop();

	//開始時間に合わせる
	data[dataNumber].fbxUpdate.nowTime = data[dataNumber].fbxUpdate.startTime;

	//再生可能
	data[dataNumber].fbxUpdate.isAnimation = true;
}

int Fbx::LoadFbx(const std::string fileName)
{
	//現在の配列番号
	const int dataNumber = (UINT)data.size();

	//空のデータを一つ増やす
	Data addData;
	data.push_back(addData);

	//名前の登録
	Fbx::fileName = fileName;

	//ファイルの型
	const string shape = ".fbx";

	//string型で結合
	const string strFileName = directoryPath + fileName + '/' + fileName + shape;

	//const char*に変換
	const char* cFileName = strFileName.c_str();

	//ファイル読み込み
	data[dataNumber].fbxUpdate.fbxScene = FbxScene::Create(fbxManager, "fbxscene");
	FbxString FileName(cFileName);
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "imp");
	fbxImporter->Initialize(FileName.Buffer(), -1, fbxManager->GetIOSettings());
	fbxImporter->Import(data[dataNumber].fbxUpdate.fbxScene);
	fbxImporter->Destroy();

	//ノード読み込み
	LoadNode(dataNumber, data[dataNumber].fbxUpdate.fbxScene->GetRootNode());

	LoadAnimation(dataNumber);

	return dataNumber;
}

void Fbx::StaticInitialize(ID3D12Device* device)
{
	HRESULT result = S_FALSE;

	// 再初期化チェック
	assert(!Fbx::device);

	// nullptrチェック
	assert(device);

	Fbx::device = device;

	CreateGraphicsPipeline();

	//デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = textureNum;
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));

	fbxManager = FbxManager::Create();

	//フレーム設定
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
}

void Fbx::Initialize()
{
	HRESULT result = S_FALSE;

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * data[modelNumber].vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * data[modelNumber].indices.size());

	//頂点バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//インデックスバッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB), // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	assert(SUCCEEDED(result));

	//頂点バッファへのデータ転送
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(data[modelNumber].vertices.begin(), data[modelNumber].vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);

	//インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	std::copy(data[modelNumber].indices.begin(), data[modelNumber].indices.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

	//頂点バッファビューの生成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(data[modelNumber].vertices[0]);

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	// 定数バッファ1の生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform));
	assert(SUCCEEDED(result));

	//定数バッファSkinの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin));
	assert(SUCCEEDED(result));
}

std::unique_ptr<Fbx> Fbx::Create(UINT modelNumber)
{
	// 3Dオブジェクトのインスタンスを生成
	Fbx* instance = new Fbx();

	//テクスチャ番号コピー
	instance->modelNumber = modelNumber;

	//初期化
	instance->Initialize();

	return std::unique_ptr<Fbx>(instance);
}

void Fbx::Update()
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	//アニメーション
	if (isAnimation)
	{
		data[modelNumber].fbxUpdate.nowTime += frameTime;
		//最後まで行ったら先頭に戻す
		if (data[modelNumber].fbxUpdate.nowTime > data[modelNumber].fbxUpdate.stopTime)
		{
			data[modelNumber].fbxUpdate.nowTime = data[modelNumber].fbxUpdate.startTime;
		}
	}

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

	// 定数バッファ1へデータ転送
	ConstBufferDataTransform* constMap = nullptr;
	result = constBuffTransform->Map(0, nullptr, (void**)&constMap);
	constMap->viewproj = camera->GetView() * camera->GetProjection();// 行列の合成
	constMap->world = matWorld;
	constMap->cameraPos = camera->GetEye();
	constMap->color = { 1.0f,1.0f,1.0f,1.0f };
	constBuffTransform->Unmap(0, nullptr);

	//ボーン配列取得
	vector<Bone>& bones = data[modelNumber].bones;

	// 定数バッファSkinへデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < bones.size(); i++)
	{
		//今の姿勢行列
		XMMATRIX matCurrentPose;
		//現在の姿勢を取得
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(data[modelNumber].fbxUpdate.nowTime);
		//XMMATRIXに変換
		ConvertMatrixFormFbx(&matCurrentPose, fbxCurrentPose);
		//合成してスキニング行列に保存
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}
	constBuffSkin->Unmap(0, nullptr);
}

void Fbx::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	Fbx::cmdList = cmdList;

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipeline->pipelineState.Get());

	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipeline->rootSignature.Get());

	//プリミティブ形状の設定コマンド
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//デスクリプタヒープをセット
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

}

void Fbx::PostDraw()
{
	// コマンドリストを解除
	Fbx::cmdList = nullptr;
}

void Fbx::Draw(int modelNumber)
{
	//二回目の呼び出しから呼ばれない
	if (this->modelNumber != modelNumber)
	{
		Create(modelNumber);
	}

	//頂点バッファの設定
	cmdList->IASetIndexBuffer(&ibView);

	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffTransform->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootConstantBufferView(2, constBuffSkin->GetGPUVirtualAddress());

	//シェーダーリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			modelNumber,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//描画コマンド
	cmdList->DrawIndexedInstanced((UINT)data[modelNumber].indices.size(), 1, 0, 0, 0);
}

std::string Fbx::ExtractFileName(const std::string& path)
{
	size_t pos1;

	//区切り文字'\\'が出てくる一番最後の部分を検索

	pos1 = path.rfind('\\');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	//区切り文字'/'が出てくる一番最後の部分を検索
	pos1 = path.rfind('/');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}

void Fbx::ConvertMatrixFormFbx(DirectX::XMMATRIX* dst, const FbxMatrix& src)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			dst->r[i].m128_f32[j] = (float)src.Get(i, j);
		}
	}
}

void Fbx::Finalize()
{
	descHeap.Reset();
	for (int i = 0; i < textureNum; i++)
	{
		texBuffer[i].Reset();
	}
	pipeline.reset();
}