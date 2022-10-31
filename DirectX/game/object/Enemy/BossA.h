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
		CIRCLE_UP_DOWN,//円状に上下する
		FIREWORKE,//花火的なもの
		//BAEM,//ビーム
		HOMING,//ホーミング
		BOMB_HOMING,//爆破してから追従
		SNAKE,//うねうね動く
		HOMING_LINE1,//追従線(円形に出る)
		HOMING_LINE2,//追従線(追従する)
		SIZE,//合計
	};

	static const int homing_line_num = 6;

	//一回分の攻撃情報
	struct BULLET_INFO
	{
		//弾の種類
		int kind;
		//1前フレームの弾の種類
		int oldKind;
		//HOMING_LINE用の座標
		XMFLOAT3 HOMING_LINEpos[homing_line_num];
		//角度ホーミング線用角度
		XMFLOAT2 radiun[homing_line_num];
		//回転軸
		XMFLOAT3 rota = { 0,0,0 };
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
	//ボスの最大体力
	const int maxHp = 400;
	//片羽の総量
	static const int wing_num = 5;

private:

	//タイマー
	int timer;
	//地面についているか
	bool onGround;
	//一回に出す弾の量
	const int bulletNum = 36;
	//一回分の攻撃情報
	std::array<BULLET_INFO, kindNum> attack;
	//敵を召喚するカウント
	int summonEnemyCount;
	//ビーム
	std::unique_ptr<BossBeam> baem;

public:

	int GetMaxHp() { return maxHp; }
	int GetHp() { return hp; }
};
