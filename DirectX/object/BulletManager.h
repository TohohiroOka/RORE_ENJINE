#pragma once
#include "FixedTurretBullet.h"
#include "EnemyABullet.h"

class BulletManager
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;

public:

	/// <summary>
	/// ����
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<BulletManager> Create();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Draw();

	/// <summary>
	/// ������
	/// </summary>
	void Reset();
	
	/// <summary>
	/// �e�ƒn�`�A�v���C���[�Ƃ̓����蔻��
	/// </summary>
	/// <param name="_pos">�v���C���[���W</param>
	bool CheckCollision(const XMFLOAT3& _pos);

private:

	//�Œ�C��̒e
	static std::vector<std::unique_ptr<FixedTurretBullet>> fixedTurretBullet;
	//�G�l�~�[A�̒e
	static std::vector<std::unique_ptr<EnemyABullet>> enemyABullet;


public:

	//�e�Z�b�g
	static void SetFixedTurretBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec){
		fixedTurretBullet.emplace_back(FixedTurretBullet::Create(_pos, _moveVec));
	}

	//�e�Z�b�g
	static void SetEnemyABullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec) {
		enemyABullet.emplace_back(EnemyABullet::Create(_pos, _moveVec));
	}

};