#include "BulletManager.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

std::vector<std::unique_ptr<FixedTurretBullet>> BulletManager::fixedTurretBullet;
std::vector<std::unique_ptr<EnemyABullet>> BulletManager::enemyABullet;
std::vector<std::unique_ptr<PlayerBullet>> BulletManager::playerBullet;

BulletManager::~BulletManager()
{
	//固定砲台の弾
	fixedTurretBullet.clear();
	std::vector<std::unique_ptr<FixedTurretBullet>>().swap(fixedTurretBullet);
	//エネミーAの弾
	enemyABullet.clear();
	std::vector<std::unique_ptr<EnemyABullet>>().swap(enemyABullet);

	BaseBullet::Finalize();
}

std::unique_ptr<BulletManager> BulletManager::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	BulletManager* instance = new BulletManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();

	return std::unique_ptr<BulletManager>(instance);
}

void BulletManager::Initialize()
{
	BaseBullet::StaticInitialize();
}

void BulletManager::Update()
{
	for (auto& i : fixedTurretBullet)
	{
		i->Update();
	}
	for (auto& i : enemyABullet)
	{
		i->Update();
	}
	for (auto& i : playerBullet)
	{
		i->Update();
	}

	//停止した弾を随時消去
	{
		const int ft_size = int(fixedTurretBullet.size());
		for (int i = 0; i < ft_size; i++)
		{
			if (!fixedTurretBullet[i]->GetIsAlive())
			{
				fixedTurretBullet.erase(fixedTurretBullet.begin() + i);
				break;
			}
		}
		const int eA_size = int(enemyABullet.size());
		for (int i = 0; i < eA_size; i++)
		{
			if (!enemyABullet[i]->GetIsAlive())
			{
				enemyABullet.erase(enemyABullet.begin() + i);
				break;
			}
		}
		const int p_size = int(playerBullet.size());
		for (int i = 0; i < p_size; i++)
		{
			if (!playerBullet[i]->GetIsAlive())
			{
				playerBullet.erase(playerBullet.begin() + i);
				break;
			}
		}
	}
}

void BulletManager::Draw()
{
	for (auto& i : fixedTurretBullet)
	{
		i->Draw();
	}
	for (auto& i : enemyABullet)
	{
		i->Draw();
	}
	for (auto& i : playerBullet)
	{
		i->Draw();
	}
}

void BulletManager::Reset()
{
	fixedTurretBullet.clear();
}

bool BulletManager::CheckEnemyBCollision(const XMFLOAT3& _pos)
{
	//衝突用に座標と半径の大きさを借りる
	XMFLOAT3 playerPos = _pos;
	playerPos.y += 10.0f;
	float playerSize = 5;

	bool isHit = false;

	//弾とプレイヤーが衝突状態でないなら抜ける
	for (auto& i : fixedTurretBullet)
	{
		if (!i->GetIsAlive()) { continue; }

		//プレイヤーと当たっていたら消す
		if (Collision::CheckCircle2Circle(
			i->GetPosition(), i->GetScale(), playerPos, playerSize)) {
			isHit = true;
			i->SetIsAlive(false);
		}
	}
	for (auto& i : enemyABullet)
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

bool BulletManager::CheckPlayerBCollision(const XMFLOAT3& _pos, float _scale)
{
	XMFLOAT3 enemyPos = _pos;
	bool hit = false;

	for (auto& itr : playerBullet)
	{
		if (!itr->GetIsAlive()) { continue; }

		//弾情報
		XMFLOAT3 bulletPos = itr->GetPosition();
		float bulletSize = itr->GetScale();
		XMFLOAT3 vec = itr->GetMoveVec();

		//移動方向のレイ情報
		Ray ray;
		ray.start = { bulletPos.x,bulletPos.y,bulletPos.z,0 };
		ray.dir = { vec.x,vec.y,vec.z,0 };

		//エネミーのcollider
		Sphere sphere;
		sphere.center = { enemyPos.x,enemyPos.y,enemyPos.z,0 };
		sphere.radius = _scale / 2.0f;
		if (Collision::CheckRay2Sphere(ray, sphere))
		{
			float x = powf(enemyPos.x - bulletPos.x, 2);
			float y = powf(enemyPos.y - bulletPos.y, 2);
			float z = powf(enemyPos.z - bulletPos.z, 2);

			float eneTobul1 = sqrt(x + y + z);
			XMFLOAT3 beforepos = itr->GetMove();
			x = powf(bulletPos.x - beforepos.x - bulletPos.x, 2);
			y = powf(bulletPos.y - beforepos.y - bulletPos.y, 2);
			z = powf(bulletPos.z - beforepos.z - bulletPos.z, 2);

			float eneTobul2 = sqrt(x + y + z);

			if (eneTobul1 < eneTobul2)
			{
				hit = true;
				itr->SetIsAlive(false);
			}
		}
	}

	return hit;
}
