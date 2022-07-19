#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class BaseCollider;
class Camera;
class LightGroup;
class GraphicsPipelineManager;
class Model;
class Texture;

class InterfaceObject3d
{
protected:// エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス

	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		XMFLOAT4 color;
		XMMATRIX viewproj; // ビュープロジェクション行列
		XMMATRIX world; // ワールド行列
		XMFLOAT3 cameraPos; // カメラ座標（ワールド座標）
		unsigned int isSkinning;//スキニングを行うか
		unsigned int isBloom;//ブルームの有無
		unsigned int isToon;//トゥーンの有無
		unsigned int isOutline;//アウトラインの有無
	};

	// 定数バッファ用データ構造体B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 baseColor;//ベースカラ―
		float pad1; // パディング
		XMFLOAT3 ambient; // アンビエント係数
		float pad2; // パディング
		XMFLOAT3 diffuse; // ディフューズ係数
		float metalness; // 金属度(0 or 1)
		float specular; // 鏡面反射度
		float roughness; // 粗さ
		float alpha;	// アルファ
		//float pad[3];//パディング
	};

public:

	virtual ~InterfaceObject3d() {};

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画前処理
	/// </summary>
	virtual void PreDraw() = 0;

	/// <summary>
	/// 描画後処理
	/// </summary>
	virtual void PostDraw() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 解放
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// デバイスのセット
	/// </summary>
	/// <param name="device">デバイスの</param>
	void SetDevice(ID3D12Device* device) { InterfaceObject3d::device = device; }

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera) { InterfaceObject3d::camera = camera; }

	/// <summary>
	/// ライトグループのセット
	/// </summary>
	/// <param name="lightGroup">ライトグループ</param>
	void SetLightGroup(LightGroup* light) { InterfaceObject3d::light = light; }

	/// <summary>
	/// キューブマップのセット
	/// </summary>
	/// <param name="cubetex">キューブマップテクスチャ</param>
	void SetLightGroup(Texture* cubetex) { InterfaceObject3d::cubetex = cubetex; }

protected:

	// デバイス
	ID3D12Device* device = nullptr;
	//コマンドリスト
	ID3D12GraphicsCommandList* cmdList = nullptr;
	//カメラ
	Camera* camera = nullptr;
	//ライト
	LightGroup* light = nullptr;
	//アウトラインの色
	XMFLOAT4 outlineColor = {};
	//アウトラインの幅
	float outlineWidth = 0;
	//キューブマップ
	Texture* cubetex = nullptr;
};