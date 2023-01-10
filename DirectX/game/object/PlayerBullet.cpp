#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const float _speed, const XMFLOAT3& _color)
{
	pos = _pos;
	moveVec = _moveVec;
	move = { moveVec.x * _speed,moveVec.y * _speed, moveVec.z * _speed };
	color = _color;
}

std::unique_ptr<PlayerBullet> PlayerBullet::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const float _speed, const XMFLOAT3& _color)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	PlayerBullet* instance = new PlayerBullet(_pos, _moveVec, _speed, _color);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<PlayerBullet>(instance);
}