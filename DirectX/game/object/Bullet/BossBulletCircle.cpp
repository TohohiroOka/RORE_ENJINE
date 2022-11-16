#include "BossBulletCircle.h"

using namespace DirectX;

std::array<bool, 3> BossBulletCircle::isCircleColor = { true,false,false };
XMFLOAT3 BossBulletCircle::circleColor = { 0.1f,0.1f,0.1f };

BossBulletCircle::BossBulletCircle(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const float _speed)
{
	pos = _pos;
	moveVec = _moveVec;
	move = { moveVec.x * _speed , moveVec.y * _speed , moveVec.z * _speed };
	color = circleColor;
	ColorChange();
}

std::unique_ptr<BossBulletCircle> BossBulletCircle::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec,
	const float _speed)
{
	// 3Dオブジェクトのインスタンスを生成
	BossBulletCircle* instance = new BossBulletCircle(_pos, _moveVec, _speed);
	if (instance == nullptr) {
		return nullptr;
	}

	instance->Initialize();

	return std::unique_ptr<BossBulletCircle>(instance);
}

void BossBulletCircle::ColorChange()
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
