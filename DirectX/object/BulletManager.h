#pragma once
#include "BaseBullet.h"

class BulletManager
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Draw();

private:

	std::unique_ptr<BaseBullet> bullet = nullptr;

};

