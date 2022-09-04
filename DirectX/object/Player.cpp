﻿#include "Player.h"
#include "DirectInput.h"
#include "DebugText.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

using namespace DirectX;

bool Player::isTargeting = false;
XMFLOAT3 Player::targetPos = {};

//二点間の角度算出
float GetAngle(XMFLOAT2 startPoint, XMFLOAT2 endPoint) {
	float radian = atan2f(endPoint.y - startPoint.y, endPoint.x - startPoint.x);
	float angle = XMConvertToDegrees(radian);
	return angle;
}

//二点間の距離算出
float GetDistance(XMFLOAT2 startPoint, XMFLOAT2 endPoint) {
	float x = powf(endPoint.x - startPoint.x, 2);
	float y = powf(endPoint.y - startPoint.y, 2);

	return sqrt(x + y);
}

std::unique_ptr<Player> Player::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	Player* instance = new Player();
	if (instance == nullptr) {
		return nullptr;
	}

	//モデル読み込み
	instance->model = Model::CreateFromOBJ("uma");

	// 初期化
	instance->object = Object3d::Create(instance->model.get());

	instance->Initialize();

	return std::unique_ptr<Player>(instance);
}

void Player::Initialize()
{
	object->Initialize();

	// コライダーの追加
	float radius = 5.0f;
	object->SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	object->GetCollider()->SetAttribute(COLLISION_ATTR_ALLIES);

	//SetBloom(true);

	object->SetOutline(true);
	object->SetOutlineWidth(0.002f);
	object->SetOutlineColor({ 1,1,1,1 });

	object->SetPosition({ 50,100,50 });

	//SetToon(true);
	object->SetScale({ 1,1,1 });

	bullet = PlayerBullet::Create();
}

void Player::Update()
{
	DirectInput* input = DirectInput::GetInstance();
	XMFLOAT3 position = object->GetPosition();

	//ラジアン変換
	float radiusLR = 0;
	float radiusUD = 0;

	if (!isTargeting)
	{
		radiusLR = XMConvertToRadians(cameraAngle + 90.0f);
		radiusUD = XMConvertToRadians(cameraAngle);
	}
	else
	{
		float angle = GetAngle({ position.x,position.z }, { targetPos.x,targetPos.z });
		radiusLR = XMConvertToRadians(angle - 90.0f);
		radiusUD = XMConvertToRadians(angle + 180);
	}

	const float Pspeed = 3.0f;
	//左へ移動
	if (input->PushKey(DIK_A)) {
		position.x -= Pspeed * cosf(radiusLR);
		position.z -= Pspeed * sinf(radiusLR);
	}
	//右へ移動
	if (input->PushKey(DIK_D)) {
		position.x += Pspeed * cosf(radiusLR);
		position.z += Pspeed * sinf(radiusLR);
	}
	//前へ移動
	if (input->PushKey(DIK_W)) {
		position.x -= Pspeed * cosf(radiusUD);
		position.z -= Pspeed * sinf(radiusUD);
	}
	//後へ移動
	if (input->PushKey(DIK_S)) {
		position.x += Pspeed * cosf(radiusUD);
		position.z += Pspeed * sinf(radiusUD);
	}

	// 落下処理
	if (!onGround) {
		// 下向き加速度
		const float fallAcc = -0.1f;
		const float fallVYMin = -3.0f;
		// 加速
		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);
		// 移動
		position.x += fallV.m128_f32[0];
		position.y += fallV.m128_f32[1];
		position.z += fallV.m128_f32[2];
	}
	// ジャンプ操作
	else if (input->TriggerKey(DIK_SPACE)) {
		onGround = false;
		const float jumpVYFist = 0.5f;
		fallV = { 0, jumpVYFist, 0, 0 };
	}

	if (input->PushKey(DIK_RETURN))
	{
		position.y += 5.0f;
	}

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
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	position.z += callback.move.m128_f32[2];

	// 球の上端から球の下端までのレイキャスト
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
	RAYCAST_HIT raycastHit;

	// 接地状態
	if (onGround) {
		// スムーズに坂を下る為の吸着距離
		const float adsDistance = 0.5f;
		// 接地を維持
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance)) {
			onGround = true;
			float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			position.y -= a;
		}
		// 地面がないので落下
		else {
			onGround = false;
			fallV = {};
		}
	}
	// 落下状態
	else if (fallV.m128_f32[1] <= 0.0f) {
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f)) {
			// 着地
			onGround = true;
			position.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
		}
	}

	object->SetPosition(position);
	object->Update();

	if (input->TriggerKey(DIK_SPACE))
	{
		bullet->Reset();
	}

	//弾更新
	bullet->Update();

	input = nullptr;
}

void Player::Draw()
{
	object->Draw();
	bullet->Draw();
}

void Player::BulletUpdate(const XMFLOAT3 target)
{
	XMFLOAT3 vecPt = {};
	XMFLOAT3 position = object->GetPosition();
	vecPt.x = target.x - position.x;
	vecPt.y = target.y - position.y;
	vecPt.z = target.z - position.z;

	//正規化
	float x = powf(vecPt.x, 2);
	float y = powf(vecPt.y, 2);
	float z = powf(vecPt.z, 2);
	float dist = sqrt(x + y + z);
	dist = 1.0f / dist;
	vecPt.x *= dist;
	vecPt.y *= dist;
	vecPt.z *= dist;

	bullet->SetMove(object->GetPosition(), vecPt);
}
