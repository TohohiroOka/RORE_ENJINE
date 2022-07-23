#pragma once
#include "InterfaceScene.h"
#include "Player.h"

class TestField : public InterfaceScene
{
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
	///	描画
	/// </summary>
	bool Draw() override;

	/// <summary>
	///	解放
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// imguiの表示
	/// </summary>
	void ImguiDraw() override;

	/// <summary>
	/// コンピュートシェーダーからの計算結果取得
	/// </summary>
	void GetConstbufferNum() override;

private:

	//カメラ座標
	XMFLOAT3 cameraPos = {};
	//カメラの回転
	float cameraAngle = 57;
	//カメラの高さ
	float cameraY = 0.0f;

	std::unique_ptr<Model> playerModel = nullptr;
	std::unique_ptr<Player> player = nullptr;
	std::unique_ptr<HeightMap> heightmap = nullptr;

	bool drawPlayer = true;
};