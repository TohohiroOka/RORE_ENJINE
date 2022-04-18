#include "ParticleManager.h"
#include <DirectXTex.h>
#include"Camera.h"

using namespace DirectX;
using namespace Microsoft::WRL;

ID3D12Device* ParticleManager::device;
ID3D12GraphicsCommandList* ParticleManager::cmdList;
UINT ParticleManager::descriptorHandleIncrementSize = 0;
std::unique_ptr<GraphicsPipelineManager> ParticleManager::pipeline;
ComPtr<ID3D12DescriptorHeap> ParticleManager::descHeap;
ComPtr<ID3D12Resource> ParticleManager::texBuffer[textureNum];
XMMATRIX ParticleManager::matBillboard = XMMatrixIdentity();
XMMATRIX ParticleManager::matBillboardY = XMMatrixIdentity();

ParticleManager::~ParticleManager()
{
	vertBuff.Reset();
	constBuff.Reset();
}

//XMFLOAT3同士の加算処理
const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
	XMFLOAT3 result;
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;

	return result;
}

void ParticleManager::CreateGraphicsPipeline()
{
	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // スケール
			"SCALE", 0, DXGI_FORMAT_R32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 色
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	pipeline = GraphicsPipelineManager::Create("Particle",
		GraphicsPipelineManager::OBJECT_KINDS::PARTICLE, inputLayout, _countof(inputLayout));
}

void ParticleManager::CommonCreate() {
	HRESULT result = S_FALSE;

	//デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = textureNum;
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));
}

void ParticleManager::StaticInitialize(ID3D12Device* device)
{
	// 初期化チェック
	assert(!ParticleManager::device);

	// nullptrチェック
	assert(device);

	ParticleManager::device = device;

	//共通データ生成
	CommonCreate();

	//パイプライン設定
	CreateGraphicsPipeline();
}

void ParticleManager::LoadTexture(UINT texNum, const wchar_t* filename)
{
	HRESULT result;

	//異常な番号の指定
	assert(texNum <= textureNum - 1);

	////WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImage{};

	result = LoadFromWICFile(
		filename,
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
		IID_PPV_ARGS(&texBuffer[texNum]));

	//テクスチャバッファにデータ転送
	result = texBuffer[texNum]->WriteToSubresource(
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
		texBuffer[texNum].Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), texNum,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
}

void ParticleManager::Initialize()
{
	HRESULT result = S_FALSE;

	// 頂点バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));
}

std::unique_ptr<ParticleManager> ParticleManager::Create(UINT texNumber)
{
	// 3Dオブジェクトのインスタンスを生成
	ParticleManager* instance = new ParticleManager();

	instance->texNumber = texNumber;

	// 初期化
	instance->Initialize();

	return std::unique_ptr<ParticleManager>(instance);
}

void ParticleManager::Add(int maxFrame, XMFLOAT3 position, XMFLOAT3 velocity,
	XMFLOAT3 accel, float startScale, float endScale, XMFLOAT4 startColor, XMFLOAT4 endColor)
{
	//リストに要素を追加
	particle.emplace_front();
	//追加した要素の参照
	Particle& p = particle.front();
	//値のリセット
	p.position = position;
	p.velocity = velocity;
	p.accel = accel;
	p.num_frame = maxFrame;
	p.scale = startScale;
	p.s_scale = startScale;
	p.e_scale = endScale;
	p.color = startColor;
	p.s_color = startColor;
	p.e_color = endColor;
}

XMMATRIX ParticleManager::UpdateViewMatrix(Camera* camera)
{
	//注意点
	XMVECTOR eyePosition = XMLoadFloat3(&camera->GetEye());
	//注意点座標
	XMVECTOR targetPosition = XMLoadFloat3(&camera->GetTarget());
	//上方向
	XMVECTOR upVector = XMLoadFloat3(&camera->GetUp());

	//カメラZ軸
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);

	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3Equal(upVector, XMVectorZero()));

	//ベクトル正規化
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	//カメラのX軸
	XMVECTOR cameraAxisX;
	//X軸は上方向→Z軸の外積で求まる
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	//ベクトル正規化
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	//カメラのY軸
	XMVECTOR cameraAxisY;
	//Y軸はX軸→Z軸の外積で求まる
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
	//ベクトル正規化
	cameraAxisY = XMVector3Normalize(cameraAxisY);

	//カメラ回転行列
	XMMATRIX matCameraRot;
	//カメラ座標系
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

	//転置
	XMMATRIX matView = XMMatrixTranspose(matCameraRot);

	//視点座標に-1を掛けた座標
	XMVECTOR reverssEyePosition = XMVectorNegate(eyePosition);
	//カメラの位置からワールド原点への変換
	XMVECTOR tX = XMVector3Dot(matCameraRot.r[0], reverssEyePosition);
	XMVECTOR tY = XMVector3Dot(matCameraRot.r[1], reverssEyePosition);
	XMVECTOR tZ = XMVector3Dot(matCameraRot.r[2], reverssEyePosition);
	//一つのベクトルにまとめる
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

	//ビュー変換に平行移動成分を設定
	matView.r[3] = translation;

#pragma region 線方向ビルボード行列の計算
	//ビルボード行列
	matBillboard.r[0] = cameraAxisX;
	matBillboard.r[1] = cameraAxisY;
	matBillboard.r[2] = cameraAxisZ;
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma region

#pragma region Y軸回りのビルボード行列の計算
	//カメラX軸,Y軸、Z軸
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	//X軸は共通
	ybillCameraAxisX = cameraAxisX;
	//Y軸はワールド座標系のY軸
	ybillCameraAxisY = XMVector3Normalize(upVector);
	//Z軸はX軸→Y軸の外積で求まる
	ybillCameraAxisZ = XMVector3Cross(cameraAxisY, cameraAxisX);
	;
	//Y軸回りビルボード行列
	matBillboardY.r[0] = ybillCameraAxisX;
	matBillboardY.r[1] = ybillCameraAxisY;
	matBillboardY.r[2] = ybillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma region

	return matView;
}

int ParticleManager::Update(Camera* camera)
{
	HRESULT result;

	//表示時間をが過ぎたパーティクルを削除
	particle.remove_if([](Particle& x) {
		return x.frame >= x.num_frame;
		}
	);

	//全パーティクル更新
	for (std::forward_list<Particle>::iterator it = particle.begin();
		it != particle.end(); it++) {
		//経過フレーム数をカウント
		it->frame++;
		//速度に加速度を加算
		it->velocity = it->velocity + it->accel;
		//速度による移動
		it->position = it->position + it->velocity;
		//大きさの変更
		it->scale = it->scale - (it->s_scale - it->e_scale) / it->num_frame;
		//色の変更
		it->color.x = it->color.x - (it->s_color.x - it->e_color.x) / it->num_frame;//赤
		it->color.y = it->color.y - (it->s_color.y - it->e_color.y) / it->num_frame;//緑
		it->color.z = it->color.z - (it->s_color.z - it->e_color.z) / it->num_frame;//青
		it->color.w = it->color.w - (it->s_color.w - it->e_color.w) / it->num_frame;//明度
	}

	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	int count = 0;
	if (SUCCEEDED(result)) {
		//パーティクルの情報を一つずつ反映
		for (std::forward_list<Particle>::iterator it = particle.begin();
			it != particle.end(); it++) {
			if (count >= vertexCount) { break; }
			//座標
			vertMap->pos = it->position;
			//スケール
			vertMap->scale = it->scale;
			//色
			vertMap->color = it->color;
			//次の頂点へ
			vertMap++;
			count++;
		}
	}

	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->mat = UpdateViewMatrix(camera) * camera->GetProjection();// 行列の合成
	constMap->matBillboard = matBillboard;// 行列の合成
	constBuff->Unmap(0, nullptr);

	return (int)std::distance(particle.begin(), particle.end());
}

void ParticleManager::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(ParticleManager::cmdList == nullptr);

	// コマンドリストをセット
	ParticleManager::cmdList = cmdList;

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipeline->pipelineState.Get());

	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipeline->rootSignature.Get());

	//プリミティブ形状の設定コマンド
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

}

void ParticleManager::PostDraw()
{
	// コマンドリストを解除
	ParticleManager::cmdList = nullptr;
}

void ParticleManager::Draw()
{
	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//デスクリプタヒープをセット
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//シェーダーリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//描画コマンド
	cmdList->DrawInstanced((UINT)std::distance(particle.begin(), particle.end()), 1, 0, 0);
}

void ParticleManager::ParticlAllDelete()
{
	particle.clear();
}

void ParticleManager::Finalize()
{
	for (int i = 0; i < textureNum; i++)
	{
		texBuffer[i].Reset();
	}
	descHeap.Reset();
	pipeline.reset();
}