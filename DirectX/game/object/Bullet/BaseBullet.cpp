#include "BaseBullet.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "GameHelper.h"

using namespace DirectX;

std::unique_ptr<Model> BaseBullet::model;
std::array<std::unique_ptr<InstanceObject>, BaseBullet::object_max_num> BaseBullet::object;
int BaseBullet::usingNum = 0;

void BaseBullet::StaticInitialize()
{
	//弾
	model = Model::CreateFromOBJ("Square1");

	for (auto& i : object)
	{
		i = InstanceObject::Create(model.get());
		i->SetBloom(true);
		i->SetLight(false);
	}
}

void BaseBullet::Initialize()
{
	isAlive = true;
	scale = 1.5f;
	rotate = { 0,0,0 };
}

void BaseBullet::Update()
{
	if (!isAlive) { return; }

	//最大値にいったら生存フラグを消す
	if (pos.x < 0.0f || pos.x>mapSize || pos.y > 710 || pos.y < -1.0f || pos.z < 0.0f || pos.z>mapSize) {
		isAlive = false;
	}

	Ray ray;
	ray.start = { pos.x,pos.y,pos.z,1 };
	ray.dir = { moveVec.x,moveVec.y,moveVec.z,0 };
	RAYCAST_HIT raycastHit;

	//レイの距離
	float distance = sqrtf(move.x * move.x + move.y * move.y + move.z + move.z);

	//カプセル
	Capsule capsule;
	capsule.startPosition = { pos.x,pos.y,pos.z };
	capsule.endPosition = { pos.x + move.x,pos.y + move.y,pos.z + move.z };
	capsule.radius = scale;
	if (CollisionManager::GetInstance()->QueryCapsule(capsule, COLLISION_ATTR_LANDSHAPE))
	{
		isAlive = false;
	}

	pos.x += move.x;
	pos.y += move.y;
	pos.z += move.z;

	for (auto& i : object)
	{
		if (!i->GetInstanceDrawCheck()) { continue; }
		i->DrawInstance(pos, { scale,scale,scale }, rotate, { color.x,color.y,color.z,1.0f });
		return;
	}
}

void BaseBullet::Draw()
{
	for (auto& i : object) {
		if (i->GetInstanceDrawNum() == 0) { continue; }
		i->Draw();
	}
}

void BaseBullet::Finalize()
{
	model.reset();
}