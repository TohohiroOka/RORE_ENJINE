#pragma once
#include "BaseEnemy.h"

class BossA : public BaseEnemy
{
public:

	BossA(const XMFLOAT3& _pos);
	~BossA() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <returns></returns>
	static std::unique_ptr<BossA> Create(const XMFLOAT3& _pos);

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:

	//タイマー
	int timer;
	//地面についているか
	bool onGround;
	//一回に出す弾の量
	const int bulletNum = 10;
};
