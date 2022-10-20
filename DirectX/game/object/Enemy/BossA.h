#pragma once
#include "BaseEnemy.h"
#include "BossBeam.h"

class BossA : public BaseEnemy
{
private:

	enum class BULLET_KIND
	{
		CIRCLE,//円状に出る
		FIREWORKE,//花火的なもの
		BAEM,//ビーム
		NUM,//合計
	};

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

	/// <summary>
	/// 更新
	/// </summary>
	void Draw() override;

private:

	//タイマー
	int timer;
	//地面についているか
	bool onGround;
	//一回に出す弾の量
	const int bulletNum = 36;
	//xz平面角度
	float angleXZ;
	//弾の種類
	int kind = int(BULLET_KIND::CIRCLE);
	//ビーム
	std::unique_ptr<BossBeam> baem;
};
