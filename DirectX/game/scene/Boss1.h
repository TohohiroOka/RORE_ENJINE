#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"
#include <array>

#include "BulletManager.h"
#include "EnemyManager.h"
#include "BossA.h"

#include "UiManager.h"

class Boss1 : public InterfaceScene
{
private:

	enum class SCENE {
		SET,//準備
		PLAY,//ゲームプレイ
		HOME,//ホーム画面
	};

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

private:

	//シーン
	SCENE scene;
	//カメラの回転
	float cameraAngle = -90;
	//カメラの高さ
	const float cameraY = 5.0f;

	//プレイヤー
	std::unique_ptr<Player> player;

	//地形
	std::unique_ptr<Ground> ground;

	//弾
	std::unique_ptr<BulletManager> bullet;
	
	//敵
	std::unique_ptr<EnemyManager> enemy;

	//ボス
	std::unique_ptr<BossA> boss;

	//UI
	std::unique_ptr<UiManager> ui;
};