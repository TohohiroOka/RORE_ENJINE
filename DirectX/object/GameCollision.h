#pragma once

#include "Ground.h"
#include "Player.h"
#include "BulletManager.h"
#include "Collision.h"

/// <summary>
/// ゲームオブジェクト当たり判定クラス
/// </summary>
class GameCollision
{
public:
	/// <summary>
	/// 敵の当たり判定
	/// </summary>
	/// <param name="player">プレイヤー</param>
	/// <param name="enemy">敵</param>
	/// <returns>衝突しているか否か</returns>
	static bool CheckBullet(Ground* _ground, Player* _player, BulletManager* _bullet);

	/// <summary>
	/// プレイヤーと敵の当たり判定
	/// </summary>
	/// <param name="player">プレイヤー</param>
	/// <param name="enemy">敵</param>
	/// <returns>衝突しているか否か</returns>
	static bool CheckPlayerToEnemy(Player* player, BaseBullet* enemy);

};