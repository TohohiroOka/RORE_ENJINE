#include "BossBulletA.h"

using namespace DirectX;

std::array<bool, 3> BossBulletA::isCircleColor = { true,false,false };
XMFLOAT3 BossBulletA::circleColor = { 0.1f,0.1f,0.1f };

BossBulletA::BossBulletA(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	pos = _pos;
	moveVec = _moveVec;
}

std::unique_ptr<BossBulletA> BossBulletA::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	BossBulletA* instance = new BossBulletA(_pos, _moveVec);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();
	instance->object->SetColor({ circleColor.x,circleColor.y,circleColor.z,1.0f });
	ColorChange();

	return std::unique_ptr<BossBulletA>(instance);
}

void BossBulletA::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();

	const float speed = 15.0f;
	move = { moveVec.x * speed , moveVec.y * speed , moveVec.z * speed };
}

void BossBulletA::ColorChange()
{
	float variation = 0.01f;
	const XMFLOAT2 minmax = { 0.1f,0.9f };

	if (isCircleColor[0] && !isCircleColor[1] && !isCircleColor[2])
	{
		circleColor.x += variation;
		if (circleColor.x >= minmax.y)
		{
			circleColor.x = minmax.y;
			isCircleColor[1] = true;
		}
	}
	else if (isCircleColor[0] && isCircleColor[1] && !isCircleColor[2])
	{
		circleColor.y += variation;
		if (circleColor.y >= minmax.y)
		{
			circleColor.y = minmax.y;
			isCircleColor[2] = true;
		}
	}
	else if (isCircleColor[0] && isCircleColor[1] && isCircleColor[2])
	{
		circleColor.z += variation;
		if (circleColor.y >= minmax.y)
		{
			circleColor.z = minmax.y;
			isCircleColor[0] = false;
		}
	}
	else if (!isCircleColor[0] && isCircleColor[1] && isCircleColor[2])
	{
		circleColor.x -= variation;
		if (circleColor.x <= minmax.x)
		{
			circleColor.x = minmax.x;
			isCircleColor[1] = false;
		}
	}
	else if (!isCircleColor[0] && !isCircleColor[1] && isCircleColor[2])
	{
		circleColor.y -= variation;
		if (circleColor.y <= minmax.x)
		{
			circleColor.y = minmax.x;
			isCircleColor[2] = false;
		}
	}
	else if (!isCircleColor[0] && !isCircleColor[1] && !isCircleColor[2])
	{
		circleColor.z -= variation;
		if (circleColor.z <= minmax.x)
		{
			circleColor.z = minmax.x;
			isCircleColor[0] = true;
		}
	}
}