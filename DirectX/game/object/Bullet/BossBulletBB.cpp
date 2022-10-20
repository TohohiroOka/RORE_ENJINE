#include "BossBulletBB.h"

using namespace DirectX;

BossBulletBB::BossBulletBB(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	pos = _pos;
	moveVec = _moveVec;
}

std::unique_ptr<BossBulletBB> BossBulletBB::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color)
{
	// 3Dオブジェクトのインスタンスを生成
	BossBulletBB* instance = new BossBulletBB(_pos, _moveVec);
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();
	instance->object->SetColor({ _color.x,_color.y,_color.z,1.0f });
	return std::unique_ptr<BossBulletBB>(instance);
}

void BossBulletBB::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();

	const float speed = 8.0f;
	move = { moveVec.x * speed , moveVec.y * speed , moveVec.z * speed };
}