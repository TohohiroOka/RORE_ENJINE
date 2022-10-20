#include "Player.h"
#include "DirectInput.h"
#include "DebugText.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"
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
	float radius = 3.0f;
	object->SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	object->GetCollider()->SetAttribute(COLLISION_ATTR_ALLIES);

	//SetBloom(true);

	object->SetOutline(true);
	object->SetOutlineWidth(0.002f);
	object->SetOutlineColor({ 1,1,1,1 });

	object->SetPosition(position);

	//SetToon(true);
	object->SetScale({ 1,1,1 });

	hp = 10;
}

void Player::Update(float _cameraAngle)
{
	DirectInput* input = DirectInput::GetInstance();

	//速度
	float Pspeed = 5.0f;
	//ラジアン変換
	cameraAngle = _cameraAngle;
	float radiusLR = XMConvertToRadians(cameraAngle + 90.0f);
	float radiusUD = XMConvertToRadians(cameraAngle);

	//二つ以上のキーが押されたとき一定以上の速度にならないように調整するためのフラグ
	std::array<bool, 4> isSpeed = { false,false, false, false };
	//左
	if (input->PushKey(DIK_A)) {
		move.x -= Pspeed * cosf(radiusLR);
		move.z -= Pspeed * sinf(radiusLR);
		isSpeed[0] = true;
	}
	//右
	if (input->PushKey(DIK_D)) {
		move.x += Pspeed * cosf(radiusLR);
		move.z += Pspeed * sinf(radiusLR);
		isSpeed[1] = true;
	}
	//前
	if (input->PushKey(DIK_W)) {
		move.x -= Pspeed * cosf(radiusUD);
		move.z -= Pspeed * sinf(radiusUD);
		isSpeed[2] = true;
	}
	//後
	if (input->PushKey(DIK_S)) {
		move.x += Pspeed * cosf(radiusUD);
		move.z += Pspeed * sinf(radiusUD);
		isSpeed[3] = true;
	}
	//上
	if (input->PushKey(DIK_UP)) {
		move.y += 2.0f;
	}
	//下
	if (input->PushKey(DIK_DOWN)) {
		move.y -= 2.0f;
	}

	//軸の異なる二方向が押されたときスピードを/2する
	if (isSpeed[0] != isSpeed[1] && isSpeed[2] != isSpeed[3])
	{
		move.x /= 2.0f;
		move.y /= 2.0f;
		move.z /= 2.0f;
	}

	position.x += move.x;
	position.y += move.y;
	position.z += move.z;

	object->SetPosition(position);
	object->Update();

	//当たり判定
	Collider();

	object->SetPosition(position);
	object->Update();

	if (input->TriggerKey(DIK_SPACE))
	{
		SetBullet();
	}

	input = nullptr;

	DebugText* text = DebugText::GetInstance();
	std::string strX = std::to_string(position.x);
	std::string strY = std::to_string(position.y);
	std::string strZ = std::to_string(position.z);
	std::string strHP = std::to_string(hp);
	text->Print("1 :: x : " + strX + "y : " + strY + "z : " + strZ, 100, 100);
	text->Print("playerHP : " + strHP, 100, 150);
	text = nullptr;

	move = { 0,0,0 };
}

void Player::Draw()
{
	object->Draw();
}

void Player::Reset()
{
	object->SetPosition({ 50,100,50 });
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

	// 球の上端から球の下端までのレイキャスト
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
	RAYCAST_HIT raycastHit;

	// スムーズに坂を下る為の吸着距離
	const float adsDistance = 1.0f;

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

void Player::SetBullet()
{
	float radiusUD = XMConvertToRadians(cameraAngle);
	XMFLOAT3 vecPt = {};
	vecPt.x = -cosf(radiusUD);
	vecPt.y = 0.0f;
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

	BulletManager::SetPlayerBullet(position, vecPt);
}
