#include "DrawLine3D.h"
#include "Camera.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include"Camera.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

ID3D12Device* DrawLine3D::device = nullptr;//デバイス
ID3D12GraphicsCommandList* DrawLine3D::cmdList = nullptr;//コマンドリスト
ComPtr<ID3D12PipelineState> DrawLine3D::pipelineState;//パイプラインステートオブジェクト
ComPtr<ID3D12RootSignature> DrawLine3D::rootSignature;//ルートシグネチャ

const float PI = 3.141592f;

DrawLine3D::~DrawLine3D()
{
	//バッファを解放
	vertBuff.Reset();
	indexBuff.Reset();
	constBuff.Reset();
}

void DrawLine3D::AllDelete()
{
	//ルートシグネチャとパイプラインステート解放
	pipelineState.Reset();
	rootSignature.Reset();
}

void DrawLine3D::Pipeline()
{
	HRESULT result;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	//頂点シェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/DrawLine3DVS.hlsl",  // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/DrawLine3DPS.hlsl",   // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	////頂点レイアウト配列の宣言と設定
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	////パイプラインステート設定
	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//ラスタライザステート
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//標準設定
	blenddesc.BlendEnable = true;//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//テストの値を0%使う
	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//テストの値を100%使う

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1; // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//深度ステンシルステート設定
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0レジスタ

	//ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0);//定数バッファビューとして初期化
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	//テクスチャサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//バージョン自動判定でのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);

	//ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = rootSignature.Get();

	////グラフィックスパイプラインステートの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineState));
}

DrawLine3D* DrawLine3D::Create(UINT LineNum)
{
	HRESULT result = S_FALSE;

	DrawLine3D* instance = new DrawLine3D();;
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize(LineNum)) {
		delete instance;
		assert(0);
		return nullptr;
	}

	instance->Update(NULL);

	return instance;
}

void DrawLine3D::StaticInitialize(ID3D12Device* device)
{
	DrawLine3D::device = device;

	//パイプライン設定
	Pipeline();

	pipelineState->SetName(L"DrawLine3dPipe");
	rootSignature->SetName(L"DrawLine3dRoot");
}

bool DrawLine3D::Initialize(UINT LineNum)
{
	HRESULT result = S_FALSE;

	//頂点データの要素数
	VERTEX_ARRAY_NUM = vertNum * LineNum;
	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	const UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * VERTEX_ARRAY_NUM);
	//頂点データの要素数変更
	vertices.resize(VERTEX_ARRAY_NUM);

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
		return false;
	}

	// 頂点バッファへのデータ転送
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	//頂点バッファビューの生成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	//インデック配列の要素数
	INDEX_ARRAY_NUM = indexNum * LineNum;
	//インデックスデータ全体のサイズ
	const UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * INDEX_ARRAY_NUM);

	//使うインデックスの作成
	std::vector<unsigned short> indices;
	for (int i = 0; i < INDEX_ARRAY_NUM; i++)
	{
		//仮インデックス
		unsigned short addIndex;

		//ベース + 一本で使うインデックス番号の最大値 * (現在の配列数 / 一本分の配列の最大数)
		addIndex = BASE_INDICES[i % indexNum] + 4 * (int)(i / indexNum);

		//配列の一番後ろに入れる
		indices.push_back(addIndex);
	}

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
		return false;
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

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	return true;
}

float DrawLine3D::GetAngle(XMFLOAT3 startPoint, XMFLOAT3 endPoint) {
	float angle = atan2f(endPoint.y - startPoint.y, endPoint.x - startPoint.x) * (180.0f / PI);
	return angle;
}

void DrawLine3D::SetLine(XMFLOAT3 startPoint[], XMFLOAT3 endPoint[], float width)
{
	HRESULT result;

	//幅
	XMFLOAT2 lineWidth1 = {};
	XMFLOAT2 lineWidth2 = {};

	//線の本数分回す
	const int LineNum = VERTEX_ARRAY_NUM / vertNum;
	for (int i = 0; i < LineNum; i++)
	{
		//角度
		const float angle = GetAngle(startPoint[i], endPoint[i]);

		//幅調整用値
		float LEFT = (angle + 90.0f) * (PI / 180.0f);
		float RIGHT = (angle - 90.0f) * (PI / 180.0f);

		lineWidth1.x = width * cosf(LEFT);
		lineWidth1.y = width * sinf(LEFT);
		lineWidth2.x = width * cosf(RIGHT);
		lineWidth2.y = width * sinf(RIGHT);

		// 頂点データ
		int arrayNum = i * 4;
		vertices[arrayNum].pos = { endPoint[i].x + lineWidth2.x, endPoint[i].y + lineWidth2.y, endPoint[i].z }; // 左下
		arrayNum++;
		vertices[arrayNum].pos = { startPoint[i].x + lineWidth2.x, startPoint[i].y + lineWidth2.y, startPoint[i].z }; // 左上
		arrayNum++;
		vertices[arrayNum].pos = { endPoint[i].x + lineWidth1.x, endPoint[i].y + lineWidth1.y, endPoint[i].z }; // 右下
		arrayNum++;
		vertices[arrayNum].pos = { startPoint[i].x + lineWidth1.x, startPoint[i].y + lineWidth1.y, startPoint[i].z }; // 右上
	}

	// 頂点バッファへのデータ転送
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}
}

void DrawLine3D::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	DrawLine3D::cmdList = cmdList;

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelineState.Get());

	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootSignature.Get());

	//プリミティブ形状の設定コマンド
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void DrawLine3D::PostDraw()
{
	// コマンドリストを解除
	DrawLine3D::cmdList = nullptr;
}

void DrawLine3D::Update(Camera* camera)
{
	//ワールド行列変換
	XMMATRIX matWorld = XMMatrixIdentity();
	XMMATRIX matScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	matWorld *= matScale;

	XMMATRIX matRot;//角度
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(0.0f));
	matRot *= XMMatrixRotationX(XMConvertToRadians(0.0f));
	matRot *= XMMatrixRotationY(XMConvertToRadians(0.0f));
	matWorld *= matRot;

	XMMATRIX matTrans;//平行方向
	matTrans = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	matWorld *= matTrans;

	//定数バッファにデータを転送
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);//マッピング
	constMap->color = color;
	constMap->matWorld = matWorld;
	if (camera != nullptr)
	{
		constMap->matView = camera->GetView();
		constMap->maProjection = camera->GetProjection();
	} else {
		constMap->matView = XMMatrixIdentity();
		constMap->maProjection = XMMatrixIdentity();
	}

	constBuff->Unmap(0, nullptr);
}

void DrawLine3D::Draw()
{
	//インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView);

	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//描画コマンド
	cmdList->DrawIndexedInstanced(INDEX_ARRAY_NUM, 1, 0, 0, 0);
}