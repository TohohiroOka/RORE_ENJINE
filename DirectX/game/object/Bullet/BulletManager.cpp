#include "BulletManager.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

std::list<std::unique_ptr<PlayerBullet>> BulletManager::playerBullet;
std::list<std::unique_ptr<BaseBullet>> BulletManager::bossBullet;

BulletManager::~BulletManager()
{
	//固定砲台の弾
	bossBullet.clear();
	std::list<std::unique_ptr<BaseBullet>>().swap(bossBullet);

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
	int aa = -1;
	for (auto& i : bossBullet)
	{
		aa++;
		if (!i->GetIsAlive()) { continue; }
		i->Update();
	}
	for (auto& i : playerBullet)
	{
		i->Update();
	}

	//停止した弾を随時消去
	{
		for (auto it = bossBullet.begin(); it != bossBullet.end();)
		{
			if (!it->get()->GetIsAlive())
			{
				// 削除された要素の次を指すイテレータが返される。
				it = bossBullet.erase(it);
			}
			// 要素削除をしない場合に、イテレータを進める
			else {
				++it;
			}
		}
		const int p_size = int(playerBullet.size());
		for (auto it = playerBullet.begin(); it != playerBullet.end();)
		{
			if (!it->get()->GetIsAlive())
			{
				// 削除された要素の次を指すイテレータが返される。
				it = playerBullet.erase(it);
			}
			// 要素削除をしない場合に、イテレータを進める
			else {
				++it;
			}
		}
	}
}

void BulletManager::Draw()
{
	for (auto& i : bossBullet)
	{
		if (!i->GetIsAlive()) { continue; }
		i->Draw();
	}
	for (auto& i : playerBullet)
	{
		if (!i->GetIsAlive()) { continue; }
		i->Draw();
	}
}

void BulletManager::Reset()
{
	bossBullet.clear();
}

bool BulletManager::CheckEnemyBCollision(const XMFLOAT3& _pos)
{
	//衝突用に座標と半径の大きさを借りる
	XMFLOAT3 playerPos = _pos;
	playerPos.y += 10.0f;
	float playerSize = 5;

	bool isHit = false;

	//弾とプレイヤーが衝突状態でないなら抜ける
	for (auto& i : bossBullet)
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
