#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"

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
	/// コンピュートシェーダーからの計算結果取得
	/// </summary>
	void GetConstbufferNum() override;

private:

	//カメラの回転
	float cameraAngle = 100;
	//カメラの高さ
	float cameraY = 50.0f;


	std::unique_ptr<Model> playerModel = nullptr;
	std::unique_ptr<Model> groundModel = nullptr;

	std::unique_ptr<Player> player = nullptr;
	std::unique_ptr<Ground> ground = nullptr;

	std::unique_ptr<TouchableObject> tmap = nullptr;

	bool isDraw = true;
};