#pragma once
#include "InterfaceScene.h"
#include "Map.h"
#include "Vector3.h"

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

	/// <summary>
	/// スクリーン座標からワールド座標への変換
	/// </summary>
	/// <param name="pout">ワールド座標</param>
	/// <param name="_screenPos">スクリーン座標</param>
	/// <param name="fZ">射影空間でのZ値（0～1</param>
	/// <returns>ワールド座標</returns>
	Vector3* CalcScreenToWorld(Vector3* _pout, XMFLOAT2 _screenPos, float fz);

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
};