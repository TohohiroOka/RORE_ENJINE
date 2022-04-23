#pragma once
#include "Object3d.h"

class FrictionObject : public Object3d
{
public:
	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<FrictionObject> Create(Model* model = nullptr);
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

	//重力
	const float gravity = 9.8f / 60.0f;
	//摩擦係数
	const float coefficientFriction = 0.8f;
	//質量(kg)
	const float mass = 10.0f;
	//垂直抗力
	const float normalForce = mass * gravity;
	//静止摩擦
	const float restFriction = coefficientFriction * normalForce;
	//移動方向にかかる力
	float movePower = 0;
};