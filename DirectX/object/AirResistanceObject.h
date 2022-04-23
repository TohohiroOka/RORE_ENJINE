#pragma once
#include "Object3d.h"

class AirResistanceObject : public Object3d
{

public:
	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<AirResistanceObject> Create(Model* model = nullptr);
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize() override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

private:

	//移動しているか
	bool onGround = true;
	//重力
	const float gravity = 9.8f / 60.0f;
	//質量(kg)
	const float mass = 5.0f;
	//比例定数
	const float k = 0.9f;
	//合計の重力
	float addG = 0.0f;
	//速度
	float speed = 0.0f;
	//加速度
	float accel = 0.0f;
};