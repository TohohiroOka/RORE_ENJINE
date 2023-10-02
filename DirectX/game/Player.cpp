#include "Player.h"
#include "Input/DirectInput.h"
#include "Object/2d/DebugText.h"
#include "Object/3d/collider/SphereCollider.h"
#include "Object/3d/collider/CollisionManager.h"
#include "Object/3d/collider/CollisionAttribute.h"
#include "Math/Vector2.h"

using namespace DirectX;

Player::Player()
{
	model = Model::CreateFromOBJ("player");
	object = Object3d::Create(model.get());
	pos = { 0.0f,0.0f,0.0f };
	moveVec = { 0.0f,0.0f,0.0f };

	// コライダーの追加
	float radius = 0.2f;
	object->SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	object->GetCollider()->SetAttribute(COLLISION_ATTR_ALLIES);

}

void Player::Update()
{
	Move();
	Collider();

	object->Update();
}

void Player::Draw()
{
	object->Draw();
}

void Player::Move()
{
	DirectInput* input = DirectInput::GetInstance();

	Vector2 raidan = Vector2(XMConvertToRadians(moveRota), XMConvertToRadians(moveRota + 90));

	//player移動
	float Pspeed = 5.0f;
	//右入力
	if (input->PushKey(DIK_D)) {
		moveVec.x += Pspeed * cosf(raidan.x);
		moveVec.z += Pspeed * cosf(raidan.y);
	}
	//左入力
	if (input->PushKey(DIK_A)) {
		moveVec.x -= Pspeed * cosf(raidan.x);
		moveVec.z -= Pspeed * cosf(raidan.y);
	}
	//下入力
	if (input->PushKey(DIK_W)) {
		moveVec.x += Pspeed * cosf(XMConvertToRadians(360.0f - moveRota + 90));
		moveVec.z += Pspeed * cosf(XMConvertToRadians(360.0f - moveRota));
	}
	//上入力
	if (input->PushKey(DIK_S)) {
		moveVec.x -= Pspeed * cosf(XMConvertToRadians(360.0f - moveRota + 90));
		moveVec.z -= Pspeed * cosf(XMConvertToRadians(360.0f - moveRota));
	}
	if (input->PushKey(DIK_LSHIFT)) {
		moveVec.y += 5.0f;
	} else if (input->PushKey(DIK_LCONTROL)) {
		moveVec.y -= 5.0f;
	}
}

void Player::Collider()
{
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(object->GetCollider());
	assert(sphereCollider);

	// クエリーコールバッククラス
	class PlayerQueryCallback : public QueryCallback
	{
	public:
		PlayerQueryCallback(Sphere* sphere) : sphere(sphere) {};

		// 衝突時コールバック関数
		bool OnQueryHit(const QUERY_HIT& info) {

			const XMVECTOR up = { 0,1,0,0 };

			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			// 地面判定しきい値
			const float threshold = cosf(XMConvertToRadians(90.0f));

			if (-threshold < cos && cos < threshold) {
				sphere->center += info.reject;
				move += info.reject;
			}

			return true;
		}

		Sphere* sphere = nullptr;
		DirectX::XMVECTOR move = {};
	};
	PlayerQueryCallback callback(sphereCollider);

	// 球と地形の交差を全検索
	CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_LANDSHAPE);
	// 交差による排斥分動かす
	pos.x += callback.move.m128_f32[0];
	pos.y += callback.move.m128_f32[1];
	pos.z += callback.move.m128_f32[2];

	{
		//	// 球の上端から球の下端までのレイキャスト
		//	Ray ray;
		//	ray.start = sphereCollider->center;
		//	ray.start.m128_f32[1] += sphereCollider->GetRadius();
		//	ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
		//	RAYCAST_HIT raycastHit;

		//	// スムーズに坂を下る為の吸着距離
		//	const float adsDistance = 1.0f;

		//	//下の判定
		//	{
		//		// 接地を維持
		//		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance)) {
		//			float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
		//			position.y -= a;
		//		}
		//	}
		//	//左の判定
		//	{
		//		ray.dir = { -1.0f,0.0f,0.0f,0.0f };
		//		// 接地を維持
		//		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
		//		{
		//			float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
		//			position.x -= a;
		//		}
		//	}
		//	//右の判定
		//	{
		//		ray.dir = { 1.0f,0.0f,0.0f,0.0f };
		//		// 接地を維持
		//		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
		//		{
		//			float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
		//			position.x += a;
		//		}
		//	}
		//	//前の判定
		//	{
		//		ray.dir = { 0.0f,0.0f,-1.0f,0.0f };
		//		// 接地を維持
		//		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
		//		{
		//			float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
		//			position.z -= a;
		//		}
		//	}
		//	//後の判定
		//	{
		//		ray.dir = { 0.0f,0.0f,1.0f,0.0f };
		//		// 接地を維持
		//		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
		//		{
		//			float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
		//			position.z += a;
		//		}
		//	}
	}

	{
		// 球の上端から球の下端までのレイキャスト
		Segment ray;
		ray.start = sphereCollider->center;
		ray.end = { pos.x + moveVec.x,pos.y + moveVec.y,pos.z + moveVec.z };
		Vector3 nMove = moveVec;
		nMove.normalize();
		ray.dir = { nMove.x, nMove.y,nMove.z,0.0f };
		RAYCAST_HIT raycastHit;

		// スムーズに坂を下る為の吸着距離
		const float adsDistance = 1.0f;

		// 接地を維持
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, moveVec.length())) {
			Vector3 a = { (raycastHit.inter.m128_f32[0] - pos.x) * 0.99f,
							(raycastHit.inter.m128_f32[1] - pos.y) * 0.99f,
							(raycastHit.inter.m128_f32[2] - pos.z) * 0.99f };
			if (abs(a.x) < 0.003f) { a.x = 0.0f; }
			if (abs(a.y) < 0.003f) { a.y = 0.0f; }
			if (abs(a.z) < 0.003f) { a.z = 0.0f; }

			pos.x += a.x;
			pos.y += a.y;
			pos.z += a.z;
		} else {
			pos.x += moveVec.x;
			pos.y += moveVec.y;
			pos.z += moveVec.z;
		}

		moveVec = { 0.0f,0.0f,0.0f };

		object->SetPosition(pos);
	}

	//position.x += moveVec[2].x;
	//position.y += moveVec[2].y;
	//position.z += moveVec[2].z;
}
