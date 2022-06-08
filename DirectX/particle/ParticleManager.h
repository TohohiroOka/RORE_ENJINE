#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include<forward_list>

#include "GraphicsPipelineManager.h"
#include "Texture.h"

class Camera;

class ParticleManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	// 頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos; // xyz座標
		float scale;//スケール
		XMFLOAT4 color;
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX matBillboard;//ビルボード行列
	};

	//パーティクル一粒
	struct Particle
	{
		//DirectX::を省略
		using XMFLOAT3 = DirectX::XMFLOAT3;

		//座標
		XMFLOAT3 position = {};
		//速度
		XMFLOAT3 velocity = {};
		//加速度
		XMFLOAT3 accel = {};
		//現在フレーム
		int frame = 0;
		//終了フレーム
		int num_frame = 0;
		//スケール
		float scale = 1.0f;
		//初期値
		float s_scale = 1.0f;
		//最終値
		float e_scale = 0.0f;
		//カラー
		XMFLOAT4 color = { 0,0,0,0 };
		//初期カラー
		XMFLOAT4 s_color = { 0,0,0,0 };
		//最終カラー
		XMFLOAT4 e_color = { 0,0,0,0 };
	};

	struct Information
	{
		bool isDelete = false; //シーン遷移で削除を行うか
		std::unique_ptr<Texture> instance = nullptr;
	};

private: // 定数

	static const int vertexCount = 512;// 頂点数
	std::forward_list<Particle>particle;

private: // 静的メンバ関数

	/// <summary>
	/// パイプライン生成
	/// </summary>
	static void CreateGraphicsPipeline();

public: // 静的メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <param name="keepName">保存名</param>
	/// <param name="filename">ファイル名</param>
	/// <param name="isDelete">シーン遷移で削除を行うか</param>
	static void LoadTexture(const std::string keepName, const std::string filename, const bool isDelete = true);

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <param name="name">テクスチャ名</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<ParticleManager> Create(const std::string name);

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	static void SetCamera(Camera* camera) {
		ParticleManager::camera = camera;
	}

	/// <summary>
	/// シーンごとの解放処理
	/// </summary>
	static void SceneFinalize();

	/// <summary>
	/// 解放処理
	/// </summary>
	static void Finalize();

private: // 静的メンバ変数

	// デバイス
	static ID3D12Device* device;
	//コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	//カメラ
	static Camera* camera;
	//パイプライン
	static std::unique_ptr<GraphicsPipelineManager> pipeline;
	//テクスチャ情報
	static std::map<std::string, Information> texture;
	//ビルボード行列
	static XMMATRIX matBillboard;
	//Y軸回りのビルボード行列
	static XMMATRIX matBillboardY;

private:// 静的メンバ関数

	/// <summary>
	/// ビュー行列の更新
	/// </summary>
	/// <returns>ビュー行列</returns>
	static XMMATRIX UpdateViewMatrix();

public: // メンバ関数

	ParticleManager() {};
	~ParticleManager();

	/// <summary>
	/// パーティクルの生成
	/// </summary>
	void Initialize();

	/// <summary>
	/// パーティクルの追加
	/// </summary>
	/// <param name="maxFrame">生存時間</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	/// <param name="accel">加速度</param>
	/// <param name="startScale">初期サイズ</param>
	/// <param name="endScale">最終サイズ</param>
	/// <param name="startColor">初期カラー</param>
	/// <param name="endColor">最終カラー</param>
	void Add(int maxFrame, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel,
		float startScale, float endScale, XMFLOAT4 startColor, XMFLOAT4 endColor);

	/// <summary>
	/// 更新
	/// </summary>
	/// <returns>配列数</returns>
	int Update();

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
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 出現しているパーティクルを全て削除する
	/// </summary>
	void ParticlAllDelete();

private: // メンバ変数

	//テクスチャ名
	std::string name;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff = {};
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// 頂点データ配列
	Vertex vertices[vertexCount] = {};
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	// ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	//テクスチャナンバー
	int texNumber = 0;

};