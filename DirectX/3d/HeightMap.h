#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include "GraphicsPipelineManager.h"
#include "Texture.h"
#include "LightGroup.h"
#include <array>
class Camera;

class HeightMap
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
		XMFLOAT3 pos; // xyz座標
		XMFLOAT3 normal; // 法線ベクトル
		XMFLOAT2 uv;  // uv座標
	};

	//定数バッファの構造体
	struct ConstBufferData {
		XMMATRIX viewproj; // ビュープロジェクション行列
		XMMATRIX world; // ワールド行列
		XMFLOAT3 cameraPos; // カメラ座標（ワールド座標）
	};

	struct HeightMapInfo {		// Heightmap structure
		int terrainWidth;		// Width of heightmap
		int terrainHeight;		// Height (Length) of heightmap
		std::vector<XMFLOAT3> heightMap;	// Array to store terrain's vertex positions
	};

	enum TEXTURE {
		HeightMapTex,
		GraphicTex1,
		GraphicTex2,
		Size,
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
	/// <param name="heightmapFilename">heightmap名</param>
	/// <param name="filename">ファイル名</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<HeightMap> Create(const std::string heightmapFilename,
		const std::string filename1 = "null", const std::string filename2 = "null");

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	static void SetCamera(Camera* camera) {
		HeightMap::camera = camera;
	}

	/// <summary>
	/// ライトグループのセット
	/// </summary>
	/// <param name="lightGroup">ライトグループ</param>
	static void SetLightGroup(LightGroup* lightGroup) {
		HeightMap::lightGroup = lightGroup;
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
	/// ハイトマップ
	/// </summary>
	/// <param name="filename">ファイル名</param>
	/// <returns>成功か</returns>
	bool HeightMapLoad(const std::string filename);

	/// <summary
	/// テクスチャ読み込み
	/// </summary>
	/// <param name="filename1">ファイル名</param>
	/// <param name="filename2">ファイル名</param>
	void LoadTexture(const std::string filename1, const std::string filename2);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

public:

	HeightMap() {};
	~HeightMap();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(UINT topology);

private:

	//デバイス
	static ID3D12Device* device;
	//コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	//パイプライン
	static std::unique_ptr<GraphicsPipelineManager> pipeline;
	//カメラ
	static Camera* camera;
	// ライト
	static LightGroup* lightGroup;
	//基礎ファイル
	static const std::string baseDirectory;

private:
	
	//テクスチャ情報
	std::array<std::unique_ptr<Texture>, TEXTURE::Size> texture;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//大きさ
	XMFLOAT3 scale = { 25,25,25 };
	//インデックスの大きさ
	int indexNum = 0;
	//頂点数
	int vertNum = 0;
	HeightMapInfo hmInfo;
};