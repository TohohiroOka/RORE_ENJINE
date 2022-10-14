#include "EnemyABullet.h"

using namespace DirectX;

EnemyABullet::EnemyABullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	pos = _pos;
	moveVec = _moveVec;
}

std::unique_ptr<EnemyABullet> EnemyABullet::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	// 3Dオブジェクトのインスタンスを生成
	EnemyABullet* instance = new EnemyABullet(_pos, _moveVec);
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();

	return std::unique_ptr<EnemyABullet>(instance);
}

void EnemyABullet::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();

	const float speed = 8.0f;
	move = { moveVec.x * speed , moveVec.y * speed , moveVec.z * speed };
}