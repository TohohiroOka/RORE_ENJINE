#include "EnemyManager.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

std::vector<std::unique_ptr<EnemyA>> EnemyManager::enemyA;

std::unique_ptr<EnemyManager> EnemyManager::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	EnemyManager* instance = new EnemyManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();

	return std::unique_ptr<EnemyManager>(instance);
}

void EnemyManager::Initialize()
{
	BaseEnemy::StaticInitialize();
	timer = 0;
}

void EnemyManager::Update(const XMFLOAT3& _pos)
{
	timer++;

	//敵追加
	if (timer % 100 == 1 && enemyA.size() < 20)
	{
		XMFLOAT3 randPos = { 1250,50,0 };
		enemyA.emplace_back(EnemyA::Create(randPos));
	}

	for (auto& i : enemyA)
	{
		i->SetMove(_pos);
		i->Update();
	}

	//停止した弾を随時消去
	const int move_effect_size = int(enemyA.size());
	for (int i = 0; i < move_effect_size; i++)
	{
		if (!enemyA[i]->GetIsAlive())
		{
			enemyA.erase(enemyA.begin() + i);
			break;
		}
	}
}

void EnemyManager::Draw()
{
	for (auto& i : enemyA)
	{
		i->Draw();
	}
}

void EnemyManager::Reset()
{
	enemyA.clear();
}

bool EnemyManager::CheckCollision(const XMFLOAT3& _pos)
{
	//衝突用に座標と半径の大きさを借りる
	XMFLOAT3 playerPos = _pos;
	playerPos.y += 10.0f;
	float playerSize = 5;

	bool isHit = false;

	//弾とプレイヤーが衝突状態でないなら抜ける
	for (auto& i : enemyA)
	{
		if (!i->GetIsAlive()) { continue; }

		//プレイヤーと当たっていたら消す
		if (Collision::CheckCircle2Circle(
			i->GetPosition(), i->GetScale(), playerPos, playerSize)) {
			isHit = true;
			i->SetIsAlive(false);
		}
	}

	return isHit;
}