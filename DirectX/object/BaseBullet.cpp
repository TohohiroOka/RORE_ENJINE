#include "BaseBullet.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

using namespace DirectX;

std::unique_ptr<Model> BaseBullet::model;

void BaseBullet::StaticInitialize()
{
	model = Model::CreateFromOBJ("Square");//プレイヤーの弾
}

void BaseBullet::Initialize()
{
	// コライダーの追加
	float radius = 3.0f;
	object->SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	object->GetCollider()->SetAttribute(COLLISION_ATTR_ALLIES);

	isAlive = true;
	scale = 3;

	object->SetBloom(true);

	object->SetScale({ scale ,scale ,scale });
	object->SetPosition(pos);
	object->Update();
}

void BaseBullet::Update()
{
	//最大値にいったら生存フラグを消す
	if (pos.x < 0.0f || pos.x>3825.0f || pos.y > 500 || pos.y < -1.0f || pos.z < 0.0f || pos.z>3825.0f) {
		isAlive = false;
	}

	{
		//SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(object->GetCollider());
		//assert(sphereCollider);

		//// クエリーコールバッククラス
		//class PlayerQueryCallback : public QueryCallback
		//{
		//public:
		//	PlayerQueryCallback(Sphere* sphere) : sphere(sphere) {};

		//	// 衝突時コールバック関数
		//	bool OnQueryHit(const QUERY_HIT& info) {

		//		const XMVECTOR up = { 0,1,0,0 };

		//		XMVECTOR rejectDir = XMVector3Normalize(info.reject);
		//		float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

		//		// 地面判定しきい値
		//		const float threshold = cosf(XMConvertToRadians(90.0f));

		//		if (-threshold < cos && cos < threshold) {
		//			sphere->center += info.reject;
		//			move += info.reject;
		//		}

		//		return true;
		//	}

		//	Sphere* sphere = nullptr;
		//	DirectX::XMVECTOR move = {};
		//};
		//PlayerQueryCallback callback(sphereCollider);

		//// 球と地形の交差を全検索
		//CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_LANDSHAPE);
		//// 交差による排斥分動かす
		//pos.x += callback.move.m128_f32[0];
		//pos.y += callback.move.m128_f32[1];
		//pos.z += callback.move.m128_f32[2];
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

	object->SetPosition(pos);
	object->Update();
}

void BaseBullet::Draw()
{
	if (!isAlive) { return; }
	object->Draw();
}

void BaseBullet::Finalize()
{
	model.reset();
}