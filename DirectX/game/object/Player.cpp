﻿#include "Player.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "GameHelper.h"
#include "Easing.h"
#include "BulletManager.h"

using namespace DirectX;

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

Player::Player(const XMFLOAT3& _pos, const XMFLOAT2& _cameraAngle)
{
	isDraw = true;
	position = _pos;
	position.y = 400.0f;
	easingPos = {};
	endEasingPos = position;
	moveVec[0] = { 0,0,0 };
	moveVec[1] = { 0,0,0 };
	speed = { 0,0,0 };
	hp = maxHp;
	moveObjAngle = { 0,0,0 };
	cameraAngle = _cameraAngle;
	isDamageStaging = false;
	damageTimer = 0;
	bulletEnergy = bulletEnergyMax;
	isMovie = false;
}

std::unique_ptr<Player> Player::Create(const XMFLOAT3& _pos, const XMFLOAT2& _cameraAngle)
{
	// 3Dオブジェクトのインスタンスを生成
	Player* instance = new Player(_pos, _cameraAngle);
	if (instance == nullptr) {
		return nullptr;
	}

	//モデル読み込み
	instance->model = Model::CreateFromOBJ("player");

	// 初期化
	instance->object = Object3d::Create(instance->model.get());

	instance->Initialize();

	return std::unique_ptr<Player>(instance);
}

void Player::Move()
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* Xinput = XInputManager::GetInstance();

	float radiusLR = XMConvertToRadians(cameraAngle.x + 90.0f);
	float radiusUD = XMConvertToRadians(cameraAngle.x);

	//最大速度
	const float maxSpeed = 2.0f;

	//二つ以上のキーが押されたとき一定以上の速度にならないように調整するためのフラグ
	std::array<bool, 4> isSpeed = { false,false, false, false };
	//左
	if (input->PushKey(DIK_A) || Xinput->LeftStickX(true)) {
		moveVec[0].x = cosf(radiusLR);
		moveVec[0].z = sinf(radiusLR);
		speed.x -= 0.5f;
		//速度制限
		if (speed.x < -maxSpeed) {
			speed.x = -maxSpeed;
		}
		//反対方向速度だった場合減速増加
		if (speed.x > 0) {
			speed.x -= fabs(speed.x) / 5.0f + 0.5f;
		}
		isSpeed[0] = true;
	}
	//右
	else if (input->PushKey(DIK_D) || Xinput->LeftStickX(false)) {
		moveVec[0].x = cosf(radiusLR);
		moveVec[0].z = sinf(radiusLR);
		speed.x += 0.5f;
		//速度制限
		if (speed.x > maxSpeed) {
			speed.x = maxSpeed;
		}
		//反対方向速度だった場合減速増加
		if (speed.x < 0) {
			speed.x += fabs(speed.x) / 5.0f + 0.5f;
		}
		isSpeed[1] = true;
	}
	//キー入力が無ければ減速
	else if (speed.x != 0) {
		bool sign = speed.x > 0;
		if (sign) {
			speed.x -= fabs(speed.x) / 5.0f;
		} else {
			speed.x += fabs(speed.x) / 5.0f;
		}

		if (fabs(speed.x) < 0.5f) {
			speed.x = 0.0f;
		}
	}
	//前
	if (input->PushKey(DIK_W) || Xinput->LeftStickY(true)) {
		moveVec[1].x = cosf(radiusUD);
		moveVec[1].z = sinf(radiusUD);
		speed.z -= 0.5f;
		//速度制限
		if (speed.z < -maxSpeed) {
			speed.z = -maxSpeed;
		}
		//反対方向速度だった場合減速増加
		if (speed.z > 0) {
			speed.z -= fabs(speed.z) / 5.0f + 0.5f;
		}
		isSpeed[2] = true;
	}
	//後
	else if (input->PushKey(DIK_S) || Xinput->LeftStickY(false)) {
		moveVec[1].x = cosf(radiusUD);
		moveVec[1].z = sinf(radiusUD);
		speed.z += 0.5f;
		//速度制限
		if (speed.z > maxSpeed) {
			speed.z = maxSpeed;
		}
		//反対方向速度だった場合減速増加
		if (speed.z < 0) {
			speed.z += (fabs(speed.z) / 5.0f) + 0.5f;
		}
		isSpeed[3] = true;
	}
	//キー入力が無ければ減速
	else if (speed.z != 0) {
		bool sign = speed.z > 0;
		if (sign) {
			speed.z -= fabs(speed.z) / 5.0f;
		} else {
			speed.z += fabs(speed.z) / 5.0f;
		}
		if (fabs(speed.z) < 0.5f) {
			speed.z = 0.0f;
		}
	}
	//上
	float moveY = 0.0f;
	if (input->PushKey(DIK_E) || Xinput->PushButton(XInputManager::PUD_BUTTON::PAD_RT)) {
		moveY = 2.0f;
	}
	//下
	if (input->PushKey(DIK_Z) || Xinput->PushButton(XInputManager::PUD_BUTTON::PAD_LT)) {
		moveY = -2.0f;
	}

	moveVec[2].x = moveVec[0].x * speed.x + moveVec[1].x * speed.z;
	moveVec[2].y = moveY;
	moveVec[2].z = moveVec[0].z * speed.x + moveVec[1].z * speed.z;

	position.x += moveVec[2].x;
	position.y += moveVec[2].y;
	position.z += moveVec[2].z;
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
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	position.z += callback.move.m128_f32[2];

	{
		// 球の上端から球の下端までのレイキャスト
		Ray ray;
		ray.start = sphereCollider->center;
		ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
		RAYCAST_HIT raycastHit;

		// スムーズに坂を下る為の吸着距離
		const float adsDistance = 1.0f;

		{
			//下の判定
			{
				// 接地を維持
				if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance)) {
					float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
					position.y -= a;
				}
			}
			//左の判定
			{
				ray.dir = { -1.0f,0.0f,0.0f,0.0f };
				// 接地を維持
				if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
				{
					float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
					position.x -= a;
				}
			}
			//右の判定
			{
				ray.dir = { 1.0f,0.0f,0.0f,0.0f };
				// 接地を維持
				if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
				{
					float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
					position.x += a;
				}
			}
			//前の判定
			{
				ray.dir = { 0.0f,0.0f,-1.0f,0.0f };
				// 接地を維持
				if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
				{
					float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
					position.z -= a;
				}
			}
			//後の判定
			{
				ray.dir = { 0.0f,0.0f,1.0f,0.0f };
				// 接地を維持
				if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
				{
					float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
					position.z += a;
				}
			}
		}
	}
}

void Player::SetBullet()
{
	float radiusUD = XMConvertToRadians(cameraAngle.x);
	XMFLOAT3 vecPt = {};
	vecPt.x = -cosf(radiusUD);
	vecPt.y = -cosf(XMConvertToRadians(cameraAngle.y));
	vecPt.z = -sinf(radiusUD);

	//正規化
	float x = powf(vecPt.x, 2);
	float y = powf(vecPt.y, 2);
	float z = powf(vecPt.z, 2);
	float dist = sqrt(x + y + z);
	dist = 1.0f / dist;
	vecPt.x *= dist;
	vecPt.y *= dist;
	vecPt.z *= dist;

	BulletManager::SetPlayerBullet(position, {0,0,1}, 10.0f, {1,1,1});
}

void Player::Initialize()
{
	object->Initialize();

	// コライダーの追加
	float radius = 3.0f;
	object->SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	object->GetCollider()->SetAttribute(COLLISION_ATTR_ALLIES);

	//SetBloom(true);

	Object3d::SetOutlineWidth(0.002f);
	Object3d::SetOutlineColor({ 1,1,1,1 });

	//object->SetOutline(true);
	object->SetPosition(position);

	//y軸の回転・オブジェクトの向き
	moveObjAngle.y = -cameraAngle.x + 90.0f;

	//y軸の回転・オブジェクトの向き
	moveObjAngle.x = cameraAngle.y - 90.0f;
	object->SetRotation(moveObjAngle);

	//SetToon(true);
	scale = 2.0f;
	object->SetScale({ scale,scale,scale });
}

void Player::Update(const DirectX::XMFLOAT2& _cameraAngle)
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* Xinput = XInputManager::GetInstance();

	if (!isMovie) {
		//ラジアン変換
		cameraAngle = _cameraAngle;

		Move();

		object->SetPosition(position);
		object->SetRotation(moveObjAngle);

		Collider();

		if ((input->TriggerKey(DIK_SPACE) || Xinput->PushButton(XInputManager::PUD_BUTTON::PAD_RB)) && bulletEnergy > 1)
		{
			SetBullet();
			bulletEnergy--;
		}
		//撃っていない時にエネルギーを回復する
		else if (bulletEnergy <= bulletEnergyMax) {
			bulletEnergy += 0.1f;
		}

		input = nullptr;
	}

	object->SetPosition(position);

	DebugText* text = DebugText::GetInstance();
	std::string strX = std::to_string(position.x);
	std::string strY = std::to_string(position.y);
	std::string strZ = std::to_string(position.z);
	std::string strMoveX = std::to_string(speed.x);
	std::string strMoveY = std::to_string(speed.y);
	std::string strMoveZ = std::to_string(speed.z);
	std::string strHP = std::to_string(hp);
	text->Print("pos :: x : " + strX + "y : " + strY + "z : " + strZ, 100, 100);
	text->Print("speed :: x : " + strMoveX + "y : " + strMoveY + "z : " + strMoveZ, 100, 125);
	text->Print("playerHP : " + strHP, 100, 150);
	text = nullptr;
}

void Player::Draw()
{
	if (isDamageStaging) {
		damageTimer++;
		if (damageTimer % 5) {
			if (isDraw)
			{
				isDraw = false;
			}
			else
			{
				isDraw = true;
			}
		}
		if (damageTimer > 50)
		{
			isDamageStaging = false;
			isDraw = true;
			damageTimer = 0;
		}
	}

	if (!isDraw) { return; }
	object->Draw();
}

void Player::Reset()
{
	object->SetPosition({ 50,100,50 });
}

void Player::SetMoviePos(const float _ratio)
{
	position.x = Easing::OutQuart(easingPos.x, endEasingPos.x, _ratio);
	position.y = Easing::OutQuart(easingPos.y, endEasingPos.y, _ratio);
	position.z = Easing::OutQuart(easingPos.z, endEasingPos.z, _ratio);
}
