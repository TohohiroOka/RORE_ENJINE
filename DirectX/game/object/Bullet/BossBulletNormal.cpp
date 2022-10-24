#include "BossBulletNormal.h"

using namespace DirectX;

BossBulletNormal::BossBulletNormal(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, float _speed)
{
	pos = _pos;
	moveVec = _moveVec;
	move = { moveVec.x * _speed , moveVec.y * _speed , moveVec.z * _speed };
}

std::unique_ptr<BossBulletNormal> BossBulletNormal::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec,
	float _speed, const XMFLOAT3& _color)
{
	// 3Dオブジェクトのインスタンスを生成
	BossBulletNormal* instance = new BossBulletNormal(_pos, _moveVec, _speed);
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();
	instance->object->SetColor({ _color.x,_color.y,_color.z,1.0f });

	return std::unique_ptr<BossBulletNormal>(instance);
}

void BossBulletNormal::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();
}