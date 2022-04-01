#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>

class ComputeShaderManager
{
protected: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private://静的メンバ変数

	// デバイス
	static ID3D12Device* device;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	//コンピュートシェーダーオブジェクト
	static ComPtr<ID3DBlob> computeShader;
	// 計算用ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootSignature;
	// 計算用パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelineState;
	//デスクリプタヒープの生成
	static ComPtr<ID3D12DescriptorHeap> Heap;
	//デスクリプタヒープの生成
	//static ComPtr<ID3D12DescriptorHeap> srvUavHeap;
	//ヒープ数
	static const UINT FrameCount = 2;
	//何とか数
	static const UINT ThreadCount = 1;
	//定数バッファの最大
	static const int IN_OUT_MAX = 300;

public:

	// 定数バッファ用構造体
	struct InputData
	{
		XMFLOAT3 startPosition[IN_OUT_MAX] = {};//初期座標
		XMFLOAT3 endPosition[IN_OUT_MAX] = {};//終了座標
		XMFLOAT3 nowPosition[IN_OUT_MAX] = {};//現在座標
		float time[IN_OUT_MAX] = {};//時間
	};

	// 定数バッファ用構造体
	struct OutputData
	{
		XMFLOAT3 nowPosition[IN_OUT_MAX] = {};
	};

private:

	/// <summary>
	/// シェーダーの読み込み
	/// </summary>
	static void LoadShader();

	/// <summary>
	/// コンピュートシェーダー用パイプラインの生成
	/// </summary>
	static void CreatePipeline();

	/// <summary>
	/// デスクリプタヒープの生成
	/// </summary>
	static void CreateDescriptor();

	/// <summary>
	/// 転送用データ生成
	/// </summary>
	void CreateBuffers();

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ComputeShaderManager() {};

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ComputeShaderManager();

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="max">配列数</param>
	/// <param name="startPosition">スタート座標配列</param>
	/// <param name="endPosition">エンド座標配列</param>
	/// <param name="nowPosition">現在座標配列</param>
	/// <param name="time">イージング時間</param>
	XMFLOAT3* ShaderUpdate(UINT max, XMFLOAT3* startPosition, XMFLOAT3* endPosition,
		XMFLOAT3* nowPosition, float* time);

	/// <summary>
	/// 更新前処理
	/// </summary>
	/// <param name="cmdList"></param>
	void PreUpdate(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 更新後処理
	/// </summary>
	void PostUpdate();

private://メンバ変数

	//定数バッファ
	ComPtr<ID3D12Resource> inputBuffer;
	//取得データの保管値
	void* data;
};