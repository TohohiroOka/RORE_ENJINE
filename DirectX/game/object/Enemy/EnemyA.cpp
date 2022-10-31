#include "EnemyA.h"
#include "BulletManager.h"
#include "DirectInput.h"
#include "GameHelper.h"

using namespace DirectX;

EnemyA::EnemyA(const XMFLOAT3& _pos)
{
	//タイマー
	timer = 0;
	//HP
	hp = 10;
	//地面についているか
	onGround = false;
	//速度
	speed = 5.0f;
	//回転角
	angle = 0;
	//オブジェクトの生成
	object = Object3d::Create(enemyAModel.get());
	//座標セット
	pos = _pos;
	object->SetPosition(pos);

	//大きさセット
	scale = 10.0f;
	object->SetScale({ scale ,scale ,scale });
}

std::unique_ptr<EnemyA> EnemyA::Create(const XMFLOAT3& _pos)
{
	// 3Dオブジェクトのインスタンスを生成
	EnemyA* instance = new EnemyA(_pos);
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();

	return std::unique_ptr<EnemyA>(instance);
}

void EnemyA::Update()
{
	timer++;

	//角度
	XMFLOAT2 objAngle = {};
	objAngle.x = GetAngle({ pos.x,pos.z }, { playerPos.x, playerPos.z });//y軸
	objAngle.y = GetAngle({ pos.x,pos.y }, { playerPos.x, playerPos.y });//z軸
	object->SetRotation({ 0,objAngle.x, objAngle.y });

	//弾の発射
	if (timer % 50 == 1)
	{
		BulletManager::SetEnemyBullet(pos, 10.0f, { 0.8f,0.2f, 0.8f });
	}

	//移動
	if (pos.y < 600)
	{
		moveVec.y = 0.5f;
	}
	else {
		moveVec.y = 0.0f;
	}

	angle += 1.3f;
	float radiun = XMConvertToRadians(angle);
	moveVec.x = cosf(radiun);
	moveVec.z = sinf(radiun);

	BaseEnemy::Update();
}