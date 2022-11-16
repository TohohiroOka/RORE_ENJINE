#pragma once
#include "Vector3.h"
#include "PlayerBullet.h"
#include "BossBulletNormal.h"
#include "BossBulletCircle.h"
#include "BossBulletFireWorke.h"
#include "BossBulletSnake.h"
#include "BossBulletHomingLine.h"
#include "BossBulletBombHoming.h"

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
	/// ���t���[������
	/// </summary>
	void EffectDraw();

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
	static void SetPlayerBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec,const float _speed, const XMFLOAT3& _color) {
		playerBullet.emplace_back(PlayerBullet::Create(_pos, _moveVec, _speed, _color));
	}

	//�e�Z�b�g
	static void SetEnemyBullet(const XMFLOAT3& _pos, const float _speed, const XMFLOAT3& _color);

	//�ʏ�e�Z�b�g
	static void SetBossBulletNormal(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec,
		const float _speed, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletNormal::Create(_pos, _moveVec, _speed, _color));
	}

	//�~��̒e�Z�b�g
	static void SetBossBulletCircle(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const float _speed) {
		bossBullet.emplace_back(BossBulletCircle::Create(_pos, _moveVec, _speed));
	}

	//�ԉΒe�Z�b�g
	static void SetBossBulletFireWorke(const XMFLOAT3& _pos, const float _speed, const XMFLOAT3& _color);

	//�z�[�~���O�e�Z�b�g
	static void SetBossBulletHoming(const XMFLOAT3& _pos, const float _speed, const XMFLOAT3& _color);

	//���ʏ�̃E�l�E�l�e�Z�b�g
	static void SetBossBulletSnake(const XMFLOAT3& _pos, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletSnake::Create(_pos, _color));
	}

	//�e�Z�b�g
	static void SetBossBulletHomingLine(XMFLOAT3& _pos, const float speed, const XMFLOAT3& _color, const int _progress);

	//�e�Z�b�g
	static void SetBossBulletHomingLine1(XMFLOAT3& _pos, const float speed, const XMFLOAT3& _color);

	//�e�Z�b�g
	static void SetBossBulletBombHoming(XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color) {
		bossBullet.emplace_back(BossBulletBombHoming::Create(_pos, _moveVec, _color));
	}

	//�e�Z�b�g
	static void SetBossBulletBombHoming1(const XMFLOAT3& _pos, const float speed, const XMFLOAT3& _color);

};