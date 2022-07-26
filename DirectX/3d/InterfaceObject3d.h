#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

#include "GraphicsPipelineManager.h"
#include "Model.h"
#include "Texture.h"
#include "CollisionInfo.h"

class BaseCollider;
class Camera;
class LightGroup;

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

protected:// サブクラス

	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
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
		XMFLOAT3 baseColor = { 1,1,1 };//ベースカラ―
		float pad1; // パディング
		XMFLOAT3 ambient; // アンビエント係数
		float pad2; // パディング
		XMFLOAT3 diffuse; // ディフューズ係数
		float metalness; // 金属度(0 or 1)
		float specular; // 鏡面反射度
		float roughness; // 粗さ
		float alpha;	// アルファ
	};

public:

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// コマンドリストのセット
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	static void SetCmdList(ID3D12GraphicsCommandList* cmdList) { InterfaceObject3d::cmdList = cmdList; }

	/// <summary>
	/// コマンドリストのセット
	/// </summary>
	static void ReleaseCmdList() { InterfaceObject3d::cmdList = nullptr; }

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	static void SetCamera(Camera* camera) { InterfaceObject3d::camera = camera; }

	/// <summary>
	/// ライトグループのセット
	/// </summary>
	/// <param name="lightGroup">ライトグループ</param>
	static void SetLightGroup(LightGroup* light) { InterfaceObject3d::light = light; }

	/// <summary>
	/// キューブマップのセット
	/// </summary>
	/// <param name="cubetex">キューブマップテクスチャ</param>
	static void SetLightGroup(Texture* cubetex) { InterfaceObject3d::cubetex = cubetex; }

public:

	InterfaceObject3d() = default;
	virtual ~InterfaceObject3d();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 当たり判定の描画
	/// </summary>
	virtual void ColliderDraw();

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateWorldMatrix();

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	virtual void OnCollision(const CollisionInfo& info) {}

protected:

	// デバイス
	static ID3D12Device* device;
	//コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	//カメラ
	static Camera* camera;
	//ライト
	static LightGroup* light;
	//アウトラインの色
	static XMFLOAT4 outlineColor;
	//アウトラインの幅
	static float outlineWidth;
	//キューブマップ
	static Texture* cubetex;

protected:

	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB0;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB1;
	//定数バッファに渡す変数
	ConstBufferDataB1 constBufferB1Num;
	//ブルームの有無
	bool isBloom = false;
	//トゥーンの有無
	bool isToon = false;
	//アウトラインの有無
	bool isOutline = false;
	//スキニング
	bool isSkinning = false;
	// モデル
	Model* model = nullptr;
	// コライダー
	BaseCollider* collider = nullptr;
	// 親オブジェクト
	InterfaceObject3d* parent = nullptr;
	// ローカルワールド変換行列
	XMMATRIX matWorld = {};
	// ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };
	// ローカル座標
	XMFLOAT3 position = { 0,0,0 };

public:

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetPosition() { return position; }

	/// <summary>
	/// 回転の取得
	/// </summary>
	/// <returns>回転</returns>
	const XMFLOAT3& GetRotation() { return rotation; }

	/// <summary>
	/// 大きさの取得
	/// </summary>
	/// <returns>大きさ</returns>
	const XMFLOAT3& GetScale() { return scale; }

	/// <summary>
	/// 座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetPosition(XMFLOAT3 position) { this->position = position; }

	/// <summary>
	/// 回転角の設定
	/// </summary>
	/// <param name="rotation">回転角</param>
	void SetRotation(XMFLOAT3 rotation) { this->rotation = rotation; }

	/// <summary>
	/// スケールの設定
	/// </summary>
	/// <param name="position">スケール</param>
	void SetScale(XMFLOAT3 scale) { this->scale = scale; }

	/// <summary>
	/// ブルームのセット
	/// </summary>
	/// <param name="isBloom">ブルーム有->true / 無->false</param>
	void SetBloom(bool isBloom) { this->isBloom = isBloom; }

	/// <summary>
	/// トゥーンのセット
	/// </summary>
	/// <param name="isToon">トゥーン有->true / 無->false</param>
	void SetToon(bool isToon) { this->isToon = isToon; }

	/// <summary>
	/// アウトラインのセット
	/// </summary>
	/// <param name="isOutline">アウトライン有->true / 無->false</param>
	void SetOutline(bool isOutline) { this->isOutline = isOutline; }

	/// <summary>
	/// アウトラインの色セット
	/// </summary>
	/// <param name="outlineColor">幅</param>
	static void SetOutlineColor(XMFLOAT4 outlineColor) { InterfaceObject3d::outlineColor = outlineColor; }

	/// <summary>
	/// アウトラインの幅セット
	/// </summary>
	/// <param name="outlineWidth">幅</param>
	static void SetOutlineWidth(float outlineWidth) { InterfaceObject3d::outlineWidth = outlineWidth; }

	/// <summary>
	/// アウトラインの色取得
	/// </summary>
	/// <param name="outlineColor">幅</param>
	static XMFLOAT4 GetOutlineColor() { return outlineColor; }

	/// <summary>
	/// アウトラインの幅取得
	/// </summary>
	/// <param name="outlineWidth">幅</param>
	static float GetOutlineWidth() { return outlineWidth; }

	/// <summary>
	/// コライダーのセット
	/// </summary>
	/// <param name="collider">コライダー</param>
	void SetCollider(BaseCollider* collider);

	/// <summary>
	/// コライダーの取得
	/// </summary>
	/// <returns>コライダー</returns>
	BaseCollider* GetCollider() { return collider; }

	/// <summary>
	/// ワールド行列の取得
	/// </summary>
	/// <returns>ワールド行列</returns>
	const XMMATRIX& GetMatWorld() { return matWorld; }

	/// <summary>
	/// 頂点のセット
	/// </summary>
	/// <param name="model">頂点</param>
	void SetMatWorld(XMMATRIX matWorld) { this->matWorld = matWorld; }

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	XMFLOAT3 GetWorldPosition();

};