#include "FixedTurretBullet.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

std::unique_ptr<FixedTurretBullet> FixedTurretBullet::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	// 3Dオブジェクトのインスタンスを生成
	FixedTurretBullet* instance = new FixedTurretBullet();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize(_pos, _moveVec);

	return std::unique_ptr<FixedTurretBullet>(instance);
}

void FixedTurretBullet::Initialize(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	object = Object3d::Create(model.get());

	isAlive = true;
	pos = _pos;
	object->SetPosition(pos);
	moveVec = _moveVec;
	scale = 3;
	object->SetScale({ scale ,scale ,scale });

	object->Update();
}

void FixedTurretBullet::Update()
{
	Ray ray;
	ray.start = { pos.x,pos.y,pos.z,0 };
	ray.dir = { moveVec.x,moveVec.y,moveVec.z,0 };
	ray.start.m128_f32[1] += scale / 2.0f;
	RAYCAST_HIT raycastHit;

	////地形に当たっている、もしくは上の最大値にいったら生存フラグを消す
	//if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, scale) ||
	//	pos.y > 500|| pos.z<0.0f) {
	//	isAlive = false;
	//}

	const float speed = 5.0f;

	pos.x += moveVec.x * speed;
	pos.y += moveVec.y * speed;
	pos.z += moveVec.z * speed;
	object->SetPosition(pos);
	object->Update();
}

void FixedTurretBullet::Draw()
{
	if(!isAlive) { return; }
	object->Draw();
}
