#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	pos = _pos;
	moveVec = _moveVec;
	const float speed = 5.0f;
	move = { moveVec.x * speed,moveVec.y * speed, moveVec.z * speed };
}

std::unique_ptr<PlayerBullet> PlayerBullet::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	PlayerBullet* instance = new PlayerBullet(_pos, _moveVec);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<PlayerBullet>(instance);
}