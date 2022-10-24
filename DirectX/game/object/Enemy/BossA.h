#pragma once
#include "BaseEnemy.h"
#include "BossBeam.h"
#include <array>

class BossA : public BaseEnemy
{
private:

	enum class BULLET_KIND
	{
		CIRCLE,//円状に出る
		CIRCLE_ROTATE,//円状に出る回転あり
		FIREWORKE,//花火的なもの
		//BAEM,//ビーム
		HOMING,//ホーミング
		BOMB_HOMING,//爆破してから追従
		SNAKE,//うねうね動く
		HOMING_LINE,//追従線
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

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

private:

	//一回にできる攻撃の種類
	static const int kindNum = 2;

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
	std::array<int, kindNum> kind;
	//1前フレームの弾の種類
	std::array<int, kindNum> oldKind;
	//ビーム
	std::unique_ptr<BossBeam> baem;
	//HOMING_LINE用の座標
	std::array<XMFLOAT3, kindNum> HOMING_LINEpos;
};
