#pragma once
#include "BaseEnemy.h"

class EnemyA : public BaseEnemy
{
public:

	EnemyA(const XMFLOAT3& _pos);
	~EnemyA() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <returns></returns>
	static std::unique_ptr<EnemyA> Create(const XMFLOAT3& _pos);

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:

	//タイマー
	int timer;
	//一回に出す弾の量
	const int bulletNum = 10;
};

