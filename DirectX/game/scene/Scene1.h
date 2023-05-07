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
	void Update() override;

	/// <summary>
	/// 更新
	/// </summary>
	void CameraUpdate(Camera* camera) override;

	/// <summary>
	/// 描画
	/// </summary>
	void DrawNotPostB() override;

	/// <summary>
	///	描画
	/// </summary>
	void Draw() override;
	
	/// <summary>
	/// 描画
	/// </summary>
	void DrawNotPostA() override;

	/// <summary>
	///	解放
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// imguiの表示
	/// </summary>
	void ImguiDraw() override;

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

	//線
	std::unique_ptr<Map> map;
	//線を描画するかのフラグ
	bool isDrawLine;
	//オブジェクト設置フラグ
	bool isAdd;
	bool frame;

	//imgui
	XMFLOAT2 imguiPos;
	const XMFLOAT2 imguiMax = { 300,300 };
	bool kaburi;
};