#include "FixedTurretBullet.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

FixedTurretBullet::FixedTurretBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	pos = _pos;
	moveVec = _moveVec;
}

std::unique_ptr<FixedTurretBullet> FixedTurretBullet::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	FixedTurretBullet* instance = new FixedTurretBullet(_pos, _moveVec);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<FixedTurretBullet>(instance);
}

void FixedTurretBullet::Initialize()
{
	object = Object3d::Create(model.get());

	const float speed = 8.0f;
	move = { moveVec.x * speed , moveVec.y * speed , moveVec.z * speed };

	BaseBullet::Initialize();
}