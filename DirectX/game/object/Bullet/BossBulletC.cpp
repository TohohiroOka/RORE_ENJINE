#include "BossBulletC.h"
#include "Easing.h"
#include "GameHelper.h"
#include "Quaternion.h"

using namespace DirectX;

BossBulletC::BossBulletC(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos)
{
	const int randMax = 50;
	pos = _pos;

	XMFLOAT3 randTargetPos = _targetPos;
	randTargetPos.x = randTargetPos.x + Randomfloat(randMax) * float(RandomSign());
	randTargetPos.y = randTargetPos.y + Randomfloat(randMax) * float(RandomSign());
	randTargetPos.z = randTargetPos.z + Randomfloat(randMax) * float(RandomSign());

	Vector3 mVec = { randTargetPos.x - pos.x, randTargetPos.y - pos.y,randTargetPos.z - pos.z };
	moveVec = mVec.normalize();
	const float speed = 20.0f;
	move = { moveVec.x * speed,moveVec.y * speed,moveVec.z * speed };
}

std::unique_ptr<BossBulletC> BossBulletC::Create(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, const XMFLOAT3& _color)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	BossBulletC* instance = new BossBulletC(_pos, _targetPos);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();
	instance->object->SetColor({ _color.x,_color.y,_color.z,1.0f });
	return std::unique_ptr<BossBulletC>(instance);
}

void BossBulletC::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();
}