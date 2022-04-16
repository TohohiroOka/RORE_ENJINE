#pragma once
#include "Object3d.h"

class FreeFallCircle : public Object3d
{
public:
	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static FreeFallCircle* Create(Model* model = nullptr);

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
	bool onGround = true;
	float addG = 0;
};