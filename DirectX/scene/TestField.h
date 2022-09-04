#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"
#include <array>

#include "EnemyManager.h"

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
	void Draw() override;

	/// <summary>
	///	解放
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// imguiの表示
	/// </summary>
	void ImguiDraw() override;

	/// <summary>
	/// カメラの更新
	/// </summary>
	void CameraUpdate();

private:

	//カメラの回転
	float cameraAngle = 100;
	//カメラの高さ
	const float cameraY = 30.0f;

	//プレイヤー
	std::unique_ptr<Player> player = nullptr;

	//地形
	static const int ground_num = 1;
	std::array<std::unique_ptr<Ground>, ground_num> ground;

	//敵
	std::unique_ptr<EnemyManager> enemy = nullptr;
};