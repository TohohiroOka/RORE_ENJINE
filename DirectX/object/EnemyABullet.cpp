#include "EnemyABullet.h"

using namespace DirectX;

EnemyABullet::EnemyABullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	pos = _pos;
	moveVec = _moveVec;
}

std::unique_ptr<EnemyABullet> EnemyABullet::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color)
{
	// 3Dオブジェクトのインスタンスを生成
	EnemyABullet* instance = new EnemyABullet(_pos, _moveVec);
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();
	instance->object->SetColor(_color);
	return std::unique_ptr<EnemyABullet>(instance);
}

void EnemyABullet::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();

	const float speed = 8.0f;
	move = { moveVec.x * speed , moveVec.y * speed , moveVec.z * speed };
}