#pragma once

#include "Ground.h"
#include "Player.h"
#include "BulletManager.h"
#include "Collision.h"

/// <summary>
/// �Q�[���I�u�W�F�N�g�����蔻��N���X
/// </summary>
class GameCollision
{
public:
	/// <summary>
	/// �G�̓����蔻��
	/// </summary>
	/// <param name="player">�v���C���[</param>
	/// <param name="enemy">�G</param>
	/// <returns>�Փ˂��Ă��邩�ۂ�</returns>
	static bool CheckBullet(Ground* _ground, Player* _player, BulletManager* _bullet);

	/// <summary>
	/// �v���C���[�ƓG�̓����蔻��
	/// </summary>
	/// <param name="player">�v���C���[</param>
	/// <param name="enemy">�G</param>
	/// <returns>�Փ˂��Ă��邩�ۂ�</returns>
	static bool CheckPlayerToEnemy(Player* player, BaseBullet* enemy);

};