#include "BossBulletA.h"

using namespace DirectX;

BossBulletA::BossBulletA(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	pos = _pos;
	moveVec = _moveVec;
}

std::unique_ptr<BossBulletA> BossBulletA::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	BossBulletA* instance = new BossBulletA(_pos, _moveVec);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();
	instance->object->SetColor({ _color.x,_color.y,_color.z,1.0f });
	return std::unique_ptr<BossBulletA>(instance);
}

void BossBulletA::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();

	const float speed = 8.0f;
	move = { moveVec.x * speed , moveVec.y * speed , moveVec.z * speed };
}