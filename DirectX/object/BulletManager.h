#pragma once
#include "FixedTurretBullet.h"
#include "EnemyABullet.h"
#include "PlayerBullet.h"

class BulletManager
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;

public:

	BulletManager() {};
	~BulletManager();

	/// <summary>
	/// 生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<BulletManager> Create();

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

	/// <summary>
	/// 初期化
	/// </summary>
	void Reset();
	
	/// <summary>
	/// 弾と地形、プレイヤーとの当たり判定
	/// </summary>
	/// <param name="_pos">プレイヤー座標</param>
	bool CheckEnemyBCollision(const XMFLOAT3& _pos);

	/// <summary>
	/// 弾と地形、エネミーとの当たり判定
	/// </summary>
	/// <param name="_pos">プレイヤー座標</param>
	/// <param name="_scale">大きさ</param>
	bool CheckPlayerBCollision(const XMFLOAT3& _pos, float _scale);

private:

	//固定砲台の弾
	static std::vector<std::unique_ptr<FixedTurretBullet>> fixedTurretBullet;
	//エネミーAの弾
	static std::vector<std::unique_ptr<EnemyABullet>> enemyABullet;
	//プレイヤーの弾
	static std::vector<std::unique_ptr<PlayerBullet>> playerBullet;

public:

	//弾セット
	static void SetFixedTurretBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec){
		fixedTurretBullet.emplace_back(FixedTurretBullet::Create(_pos, _moveVec));
	}

	//弾セット
	static void SetEnemyABullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec) {
		enemyABullet.emplace_back(EnemyABullet::Create(_pos, _moveVec));
	}

	//弾セット
	static void SetPlayerBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec) {
		playerBullet.emplace_back(PlayerBullet::Create(_pos, _moveVec));
	}

	std::vector<std::unique_ptr<PlayerBullet>>& GetPlayerBullet() {
		return playerBullet;
	}
};