#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"
#include <array>

#include "BulletManager.h"
#include "EnemyManager.h"
#include "BossA.h"

class Boss1 : public InterfaceScene
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
	float cameraAngle = -90;
	//カメラの高さ
	const float cameraY = 30.0f;
	//ターゲット
	bool isTarget;
	//ターゲット時のイージング
	bool isTageEase;
	//イージングタイマー
	float easeTimer;
	//イージング初期角度
	float startAngle;
	//イージング初期値点
	XMFLOAT3 startTarget;
	//イージング初期値点
	XMFLOAT3 startPos;

	//プレイヤー
	std::unique_ptr<Player> player = nullptr;

	//地形
	static const int ground_num = 1;
	std::array<std::unique_ptr<Ground>, ground_num> ground;

	//弾
	std::unique_ptr<BulletManager> bullet;
	
	//敵
	std::unique_ptr<EnemyManager> enemy;

	//ボス
	std::unique_ptr<BossA> boss;
};