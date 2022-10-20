#pragma once
#include "PlayerBullet.h"
#include "BossBulletA.h"
#include "BossBulletB.h"
#include "BossBulletBB.h"

class BulletManager
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;

public:

	BulletManager() {};
	~BulletManager();

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
	bool CheckEnemyBCollision(const XMFLOAT3& _pos);

	/// <summary>
	/// �e�ƒn�`�A�G�l�~�[�Ƃ̓����蔻��
	/// </summary>
	/// <param name="_pos">�v���C���[���W</param>
	/// <param name="_scale">�傫��</param>
	bool CheckPlayerBCollision(const XMFLOAT3& _pos, float _scale);

private:

	//�v���C���[�̒e
	static std::list<std::unique_ptr<PlayerBullet>> playerBullet;
	//�e�z��
	static std::list<std::unique_ptr<BaseBullet>> bossBullet;

public:

	//�e�Z�b�g
	static void SetPlayerBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec) {
		bossBullet.emplace_back(PlayerBullet::Create(_pos, _moveVec));
	}

	//�e�Z�b�g
	static void SetBossBulletA(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletA::Create(_pos, _moveVec, _color));
	}

	//�e�Z�b�g
	static void SetBossBulletB(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletB::Create(_pos, _moveVec, _color));
	}

	//�e�Z�b�g
	static void SetBossBulletBB(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletBB::Create(_pos, _moveVec, _color));
	}

	std::list<std::unique_ptr<PlayerBullet>>& GetPlayerBullet() {
		return playerBullet;
	}
};