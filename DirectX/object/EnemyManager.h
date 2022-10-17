#pragma once
#include "EnemyA.h"

class EnemyManager
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;

public:

	EnemyManager() {};
	~EnemyManager();

	/// <summary>
	/// 生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<EnemyManager> Create();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	/// <param name="_pos">プレイヤー座標</param>
	void Update(const XMFLOAT3& _pos);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 初期化
	/// </summary>
	void Reset();

	/// <summary>
	/// 弾と地形、プレイヤーとの当たり判定
	/// </summary>
	/// <param name="_player">プレイヤー座標</param>
	bool CheckCollision(const XMFLOAT3& _player);

private:

	//タイマー
	int timer;
	//敵A
	static std::vector<std::unique_ptr<EnemyA>> enemyA;
	//出現位置
	static const std::array<XMFLOAT3, 4> popPos;
	//敵Aが現フレームで倒されたかどうか
	bool isKillEnemyA;

public:

	std::vector<std::unique_ptr<EnemyA>>& GetEnemyA() {
		return enemyA;
	}

	bool GetKillBoss() {
		return isKillEnemyA;
	}
};

