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
	model = Model::CreateFromOBJ("NormalCube");
	object = Object3d::Create(model.get());
	pos = { 100.0f,200.0f,100.0f };
	moveVec = { 0.0f,0.0f,0.0f };

	// コライダーの追加
	float radius = 0.6f;
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

	// 落下処理
	if (!onGround) {
		// 下向き加速度
		const float fallAcc = -0.05f;
		const float fallVYMin = -5.0f;
		// 加速
		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);
		// 移動
		moveVec.y += fallV.m128_f32[1];
	}
	// ジャンプ操作
	else if (input->TriggerKey(DIK_SPACE)) {
		onGround = false;
		const float jumpVYFist = 2.0f;
		fallV = { 0, jumpVYFist, 0, 0 };
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
		// 球の上端から球の下端までのレイキャスト
		Segment ray;
		ray.start = sphereCollider->center;
		ray.start.m128_f32[1] += sphereCollider->GetRadius();
		ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
		RAYCAST_HIT raycastHit;

		// スムーズに坂を下る為の吸着距離
		const float adsDistance = 1.0f;

		//下の判定
		{
			// 接地を維持
			if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, moveVec.length())) {
				float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
				pos.y -= a;
				moveVec.y = 0;
				fallV = {};
				onGround = true;

			}
			// 地面がないので落下
			else {
				onGround = false;
				pos.y += moveVec.y;
			}

		}
		//左の判定
		{
			ray.dir = { -1.0f,0.0f,0.0f,0.0f };
			// 接地を維持
			if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, moveVec.x + adsDistance))
			{
				float a = (raycastHit.distance - moveVec.x);
				pos.x -= a;
				moveVec.x = 0;
			}
		}
		//右の判定
		{
			ray.dir = { 1.0f,0.0f,0.0f,0.0f };
			// 接地を維持
			if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, moveVec.x + adsDistance))
			{
				float a = (raycastHit.distance - moveVec.x);
				pos.x += a;
				moveVec.x = 0;
			}
		}
		//前の判定
		{
			ray.dir = { 0.0f,0.0f,-1.0f,0.0f };
			// 接地を維持
			if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, moveVec.z + adsDistance))
			{
				float a = (raycastHit.distance - moveVec.z);
				pos.z -= a;
				moveVec.z = 0;
			}
		}
		//後の判定
		{
			ray.dir = { 0.0f,0.0f,1.0f,0.0f };
			// 接地を維持
			if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, moveVec.z + adsDistance))
			{
				float a = (raycastHit.distance - moveVec.z);
				pos.z += a;
				moveVec.z = 0;
			}
		}
	}

	{
	//	// 球の上端から球の下端までのレイキャスト
	//	Segment ray;
	//	ray.start = sphereCollider->center;
	//	ray.end = { pos.x + moveVec.x,pos.y + moveVec.y,pos.z + moveVec.z };
	//	Vector3 nMove = moveVec;
	//	nMove.normalize();
	//	ray.dir = { nMove.x, nMove.y,nMove.z,0.0f };
	//	RAYCAST_HIT raycastHit;

	//	// スムーズに坂を下る為の吸着距離
	//	const float adsDistance = 0.2f;

	//	//// 接地を維持
	//	//if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, moveVec.length())) {
	//	//	Vector3 a = { (raycastHit.inter.m128_f32[0] - pos.x) * 0.99f,
	//	//					(raycastHit.inter.m128_f32[1] - pos.y) * 0.99f,
	//	//					(raycastHit.inter.m128_f32[2] - pos.z) * 0.99f };
	//	//	if (abs(a.x) < 0.003f) { a.x = 0.0f; }
	//	//	if (abs(a.y) < 0.003f) { a.y = 0.0f; }
	//	//	if (abs(a.z) < 0.003f) { a.z = 0.0f; }

	//	//	pos.x += a.x;
	//	//	pos.y += a.y;
	//	//	pos.z += a.z;
	//	//}

	//	// 球の上端から球の下端までのレイキャスト
	//	ray.start = { pos.x,pos.y,pos.z };
	//	ray.end = { pos.x,pos.y + moveVec.y,pos.z };
	//	ray.dir = { 0,-1,0,0 };
	//	raycastHit = {};
	//	if (onGround) {
	//		// スムーズに坂を下る為の吸着距離
	//		const float adsDistance = 0.2f;
	//		// 接地を維持
	//		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, moveVec.y + adsDistance)) {
	//			onGround = true;
	//			pos.y += (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
	//			moveVec.y = 0;
	//		}
	//	}
	//	// 落下状態
	//	else if (fallV.m128_f32[1] <= 0.0f) {
	//		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, moveVec.y)) {
	//			// 着地
	//			onGround = true;
	//			pos.y += (raycastHit.distance - moveVec.y);
	//			moveVec.y = 0;
	//		}else{
	//			pos.y += moveVec.y;
	//		}
	//	}
	}

	pos.x += moveVec.x;
	pos.z += moveVec.z;
	object->SetPosition(pos);

	moveVec={};
}
