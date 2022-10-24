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
	/// <param name="_pos">�v���C���[���W</param>
	void Update(const XMFLOAT3& _playerPos);

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
	bool CheckEnemyBulletToPlayerCollision();

	/// <summary>
	/// �e�ƒn�`�A�G�l�~�[�Ƃ̓����蔻��
	/// </summary>
	/// <param name="_pos">�v���C���[���W</param>
	/// <param name="_scale">�傫��</param>
	bool CheckPlayerBulletToEnemyCollision(const XMFLOAT3& _pos, float _scale);

private:

	//�v���C���[�̒e
	static std::list<std::unique_ptr<PlayerBullet>> playerBullet;
	//�e�z��
	static std::list<std::unique_ptr<BaseBullet>> bossBullet;
	//�v���C���[���W
	static XMFLOAT3 playerPos;

public:

	//�e�Z�b�g
	static void SetPlayerBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec) {
		playerBullet.emplace_back(PlayerBullet::Create(_pos, _moveVec));
	}

	//�e�Z�b�g
	static void SetBossBulletNormal(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec,
		const float _speed, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletNormal::Create(_pos, _moveVec, _speed, _color));
	}

	//�e�Z�b�g
	static void SetBossBulletA(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec) {
		bossBullet.emplace_back(BossBulletA::Create(_pos, _moveVec));
	}

	//�e�Z�b�g
	static void SetBossBulletB(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletB::Create(_pos, _moveVec, _color));
	}

	//�e�Z�b�g
	static void SetBossBulletC(const XMFLOAT3& _pos, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletC::Create(_pos, playerPos, _color));
	}

	//�e�Z�b�g
	static void SetBossBulletD(const XMFLOAT3& _pos, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletD::Create(_pos, _color));
	}

	//�e�Z�b�g
	static void SetBossBulletE(XMFLOAT3& _pos, const float speed, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletE::Create(_pos, _color));
		Vector3 moveVec = { playerPos.x - _pos.x,playerPos.y - _pos.y ,playerPos.z - _pos.z };
		moveVec = moveVec.normalize();
		_pos.x += moveVec.x * speed;
		_pos.y += moveVec.y * speed;
		_pos.z += moveVec.z * speed;
	}

	//�e�Z�b�g
	static void SetBossBulletF(XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletF::Create(_pos, _moveVec, _color));
	}

	//�e�Z�b�g
	static void SetBossBulletFF(const XMFLOAT3& _pos, const float speed, const XMFLOAT3& _color);

};