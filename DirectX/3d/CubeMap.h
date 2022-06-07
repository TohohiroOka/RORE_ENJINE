#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include "GraphicsPipelineManager.h"
#include "Texture.h"

class Camera;

class CubeMap
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	//頂点データ3D
	struct Vertex {
		XMFLOAT3 pos;
		XMFLOAT3 uv;
	};

	//定数バッファの構造体
	struct ConstBufferData {
		XMMATRIX viewproj; // ビュープロジェクション行列
		XMMATRIX world; // ワールド行列
		XMFLOAT3 cameraPos; // カメラ座標（ワールド座標）
	};

private:

	/// <summary>
	/// パイプライン生成
	/// </summary>
	static void CreateGraphicsPipeline();

public://メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// directXCommon生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<CubeMap> Create(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	static void SetCamera(Camera* camera) {
		CubeMap::camera = camera;
	}

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 解放処理
	/// </summary>
	static void Finalize();

private://メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

public:

	CubeMap() {};
	~CubeMap();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// キューブマップの転送
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	/// <param name="RootParameterIndex"></param>
	void TransferTextureBubber(ID3D12GraphicsCommandList* cmdList,UINT RootParameterIndex);

	/// <summary>
	/// キューブマップのセット
	/// </summary>
	/// <returns></returns>
	Texture* SetTexture() { return texture.get(); }

private:

	//デバイス
	static ID3D12Device* device;
	//コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	//パイプライン
	static std::unique_ptr<GraphicsPipelineManager> pipeline;
	//カメラ
	static Camera* camera;
	//インデックス数
	static const int indexNum = 36;

private:

	//テクスチャ情報
	std::unique_ptr<Texture> texture = nullptr;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};
	//キューブマップバッファ
	ComPtr<ID3D12Resource> texConstBuffer;
	//リソース配列
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//座標
	XMFLOAT3 position = { 0,400,0 };
	//大きさ
	XMFLOAT3 scale = { 500,500,500 };
	//回転
	XMFLOAT3 rotation = {};

};