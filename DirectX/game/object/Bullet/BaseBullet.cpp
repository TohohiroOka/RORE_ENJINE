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
	model = Model::CreateFromOBJ("Square");//プレイヤーの弾

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
	scale = 3;
	rotate = { 0,0,0 };
}

void BaseBullet::Update()
{
	if (!isAlive) { return; }

	//最大値にいったら生存フラグを消す
	if (pos.x < 0.0f || pos.x>mapX || pos.y > 500 || pos.y < -1.0f || pos.z < 0.0f || pos.z>mapZ) {
		isAlive = false;
	}

	Ray ray;
	ray.start = { pos.x,pos.y,pos.z,1 };
	ray.dir = { moveVec.x,moveVec.y,moveVec.z,0 };
	RAYCAST_HIT raycastHit;

	//レイの距離
	float distance = sqrtf(move.x * move.x + move.y * move.y + move.z + move.z);

	//レイ
	if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, distance))
	{
		XMFLOAT3 hitPos = { raycastHit.inter.m128_f32[0], raycastHit.inter.m128_f32[1], raycastHit.inter.m128_f32[2] };
		float x = powf(hitPos.x - pos.x, 2);
		float y = powf(hitPos.y - pos.y, 2);
		float z = powf(hitPos.z - pos.z, 2);
		float eneTobul1 = sqrt(x + y + z);

		//x = powf(move.x, 2);
		//y = powf(move.y, 2);
		//z = powf(move.z, 2);
		x = powf(pos.x + move.x, 2);
		y = powf(pos.y + move.y, 2);
		z = powf(pos.z + move.z, 2);

		float eneTobul2 = sqrt(x + y + z);

		if (eneTobul1 < eneTobul2)
		{
			isAlive = false;
		}
	}

	pos.x += move.x;
	pos.y += move.y;
	pos.z += move.z;

	for (auto& i : object)
	{
		if (!i->GetInstanceDrawCheck()) { continue; }
		i->DrawInstance(pos, { scale,scale,scale }, rotate, { color.x,color.y,color.z,0.5f });
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