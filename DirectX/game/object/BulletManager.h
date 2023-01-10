#pragma once
#include "Vector3.h"
#include "PlayerBullet.h"

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
	/// �e�ƒn�`�A�G�l�~�[�Ƃ̓����蔻��
	/// </summary>
	/// <param name="_pos">�v���C���[���W</param>
	/// <param name="_scale">�傫��</param>
	bool CheckPlayerBulletToEnemyCollision(const XMFLOAT3& _pos, float _scale);

private:

	//�v���C���[�̒e
	static std::list<std::unique_ptr<PlayerBullet>> playerBullet;
	//�v���C���[���W
	static XMFLOAT3 playerPos;

public:

	//�e�Z�b�g
	static void SetPlayerBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec,const float _speed, const XMFLOAT3& _color) {
		playerBullet.emplace_back(PlayerBullet::Create(_pos, _moveVec, _speed, _color));
	}
};