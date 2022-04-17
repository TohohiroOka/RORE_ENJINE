#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include<forward_list>

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

private: // 定数
	static const int vertexCount = 512;// 頂点数
	std::forward_list<Particle>particle;

public: // 静的メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void Initialize(ID3D12Device* device);

	/// <summary>
	/// パイプライン生成
	/// </summary>
	static void Pipeline();

	/// <summary>
	/// テクスチャデスクリプタの生成
	/// </summary>
	static void CommonCreate();

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <param name="texNum">テクスチャ番号</param>
	/// <param name="filename">ファイル名</param>
	static void LoadTexture(UINT texNum, const wchar_t* filename);

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <param name="texNumber">テクスチャ番号</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<ParticleManager> Create(UINT texNumber);

	/// <summary>
	/// 解放処理
	/// </summary>
	static void AllDelete();

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;
	//コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootSignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelineState;
	// デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> descHeap;
	//テクスチャ読み込み最大値
	static const int textureNum = 50;
	// テクスチャバッファ
	static ComPtr<ID3D12Resource> texBuffer[textureNum];
	//ビルボード行列
	static XMMATRIX matBillboard;
	//Y軸回りのビルボード行列
	static XMMATRIX matBillboardY;

private:// 静的メンバ関数

	//ビュー行列を更新
	static XMMATRIX UpdateViewMatrix(Camera* camera);

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
	int Update(Camera* camera);

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