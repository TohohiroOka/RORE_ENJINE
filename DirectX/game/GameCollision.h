#pragma once

#include "BulletManager.h"
#include "EnemyManager.h"
#include "Collision.h"

/// <summary>
/// �Q�[���I�u�W�F�N�g�����蔻��N���X
/// </summary>
class GameCollision
{
public:

	/// <summary>
	/// �e�ƓG�̓����蔻��
	/// </summary>
	/// <param name="_bullet">�e</param>
	/// <param name="_enemy">�G</param>
	static bool CheckBulletToEnemy(BaseBullet* _bullet, BaseEnemy* _enemy);
};