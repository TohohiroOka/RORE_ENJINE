#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class Camera;

class DrawLine3D
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://構造体宣言
	//定数バッファの構造体
	struct ConstBufferData {
		XMFLOAT4 color;//色
		XMMATRIX matWorld;//3D変換行列
		XMMATRIX matView;//3D変換行列
		XMMATRIX maProjection;//3D変換行列
	};

	//頂点データ3D
	struct Vertex {
		XMFLOAT3 pos;
	};

private://静的メンバ関数

	/// <summary>
	/// パイプライン生成
	/// </summary>
	static void Pipeline();

	/// <summary>
	/// 角度を求める
	/// </summary>
	/// <param name="startPoint">始点</param>
	/// <param name="endPoint">終点</param>
	/// <returns>二点間の角度</returns>
	float GetAngle(XMFLOAT3 startPoint, XMFLOAT3 endPoint);

public://静的メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <returns></returns>
	DrawLine3D() {};

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DrawLine3D();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// Objectの生成
	/// </summary>
	static DrawLine3D* Create();

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

public://メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="camera">カメラのインスタンス</param>
	void Update(Camera* camera);

	/// <summary>
	/// 線の情報セット
	/// </summary>
	/// <param name="startPoint">始点</param>
	/// <param name="endPoint">終点</param>
	/// <param name="color">色</param>
	/// <param name="width">線の幅</param>
	void SetLine(DirectX::XMFLOAT3 startPoint, DirectX::XMFLOAT3 endPoint, XMFLOAT4 color, float width);

	/// <summary>
	/// //描画
	/// </summary>
	void Draw();


private://静的メンバ変数

	// 頂点数
	static const int vertNum = 4;
	// インデックス数
	static const int indexNum = 12;
	//デバイス
	static ID3D12Device* device;
	//コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	//パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState>pipelineState;
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature>rootSignature;

protected://メンバ変数

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
	// 色
	XMFLOAT4 color = {};
};