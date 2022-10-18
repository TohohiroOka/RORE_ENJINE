#include "EnemyA.h"
#include "BulletManager.h"

#include "DirectInput.h"

using namespace DirectX;

EnemyA::EnemyA(const XMFLOAT3& _pos)
{
	//タイマー
	timer = 0;
	//HP
	hp = 10;
	//地面についているか
	onGround = false;
	//移動速度
	speed = 1.0f;
	//オブジェクトの生成
	object = Object3d::Create(model.get());
	//座標セット
	pos = _pos;
	object->SetPosition(pos);

	//大きさセット
	scale = 3.0f;
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

	//if (timer % 50 == 1)
	//{
	//	float angle = 360.0f / float(bulletNum);
	//	for (int i = 0; i < bulletNum; i++)
	//	{
	//		float nowAngle = angle * i;
	//		float radiun = XMConvertToRadians(nowAngle);
	//		BulletManager::SetEnemyABullet(pos, { cos(radiun),0.0f,sin(radiun) });
	//	}
	//}

	BaseEnemy::Update();
}