#pragma once
#include "Vector3.h"
#include "PlayerBullet.h"
#include "BossBulletNormal.h"
#include "BossBulletA.h"
#include "BossBulletB.h"
#include "BossBulletC.h"
#include "BossBulletD.h"
#include "BossBulletE.h"
#include "BossBulletF.h"

class BulletManager
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;

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
	/// <param name="_pos">プレイヤー座標</param>
	void Update(const XMFLOAT3& _playerPos);

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
	bool CheckEnemyBulletToPlayerCollision();

	/// <summary>
	/// 弾と地形、エネミーとの当たり判定
	/// </summary>
	/// <param name="_pos">プレイヤー座標</param>
	/// <param name="_scale">大きさ</param>
	bool CheckPlayerBulletToEnemyCollision(const XMFLOAT3& _pos, float _scale);

private:

	//プレイヤーの弾
	static std::list<std::unique_ptr<PlayerBullet>> playerBullet;
	//弾配列
	static std::list<std::unique_ptr<BaseBullet>> bossBullet;
	//プレイヤー座標
	static XMFLOAT3 playerPos;

public:

	//弾セット
	static void SetPlayerBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec) {
		playerBullet.emplace_back(PlayerBullet::Create(_pos, _moveVec));
	}

	//弾セット
	static void SetBossBulletNormal(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec,
		const float _speed, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletNormal::Create(_pos, _moveVec, _speed, _color));
	}

	//弾セット
	static void SetBossBulletA(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec) {
		bossBullet.emplace_back(BossBulletA::Create(_pos, _moveVec));
	}

	//弾セット
	static void SetBossBulletB(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletB::Create(_pos, _moveVec, _color));
	}

	//弾セット
	static void SetBossBulletC(const XMFLOAT3& _pos, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletC::Create(_pos, playerPos, _color));
	}

	//弾セット
	static void SetBossBulletD(const XMFLOAT3& _pos, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletD::Create(_pos, _color));
	}

	//弾セット
	static void SetBossBulletE(XMFLOAT3& _pos, const float speed, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletE::Create(_pos, _color));
		Vector3 moveVec = { playerPos.x - _pos.x,playerPos.y - _pos.y ,playerPos.z - _pos.z };
		moveVec = moveVec.normalize();
		_pos.x += moveVec.x * speed;
		_pos.y += moveVec.y * speed;
		_pos.z += moveVec.z * speed;
	}

	//弾セット
	static void SetBossBulletF(XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletF::Create(_pos, _moveVec, _color));
	}

	//弾セット
	static void SetBossBulletFF(const XMFLOAT3& _pos, const float speed, const XMFLOAT3& _color);

};