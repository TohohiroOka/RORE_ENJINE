#pragma once
#include "InterfaceScene.h"
#include "Vector3.h"
#include "Map.h"
#include "HeightMap.h"

class Scene1 : public InterfaceScene
{
private:// エイリアス
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMINT3 = DirectX::XMINT3;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	///	更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 更新
	/// </summary>
	void CameraUpdate(const int _cameraNum, Camera* camera) override;

	/// <summary>
	///	描画
	/// </summary>
	/// <param name="_cameraNum">カメラ番号</param>
	void Draw(const int _cameraNum) override;
	
	/// <summary>
	/// ポストエフェクトをかけない描画
	/// </summary>
	void NonPostEffectDraw(const int _cameraNum) override;

	/// <summary>
	///	解放
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// imguiの表示
	/// </summary>
	void ImguiDraw() override;

	/// <summary>
	/// 
	/// </summary>
	void FrameReset() override;

private:

	Camera* camera;

	//カメラ座標
	XMFLOAT3 cameraPos;
	//カメラ座標
	XMFLOAT3 cameraTarget;

	//マップ名
	static char fileName[36];

	//線
	std::unique_ptr<Map> map;
	//線を描画するかのフラグ
	bool isDrawLine;

	//設置オブジェクト管理/0プレイヤー/1ゴール/2ノーマル
	std::array<bool, 9> isSetObject;

	//imgui
	XMFLOAT2 imguiPos;
	const XMFLOAT2 imguiMax = { 300,300 };
	bool kaburi;

	//マップチェンジ
	bool mapChange;
	XMINT3 mapChangeSize;

	//タイマー
	int exportTimer;
	int improtTimer;
	bool isImprot;

	//imguiカラー
	XMFLOAT4 imguiColor;

	//外側の面との判定
	bool isOutsideCollision;

	//接地面の追加方向
	bool mapChangeDirection;
	
	std::unique_ptr<Model> model;

	std::unique_ptr<Sprite> sprite;

	//地形情報
	std::unique_ptr<TerrainModel> m_model;
	//描画設定
	std::unique_ptr<HeightMap> object;

};