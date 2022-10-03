#include "GameCollision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

using namespace DirectX;

bool GameCollision::CheckBulletToEnemy(std::vector<std::unique_ptr<BaseBullet>>* bullet, BaseEnemy* enemy)
{
	//Õ“Ë—p‚ÉÀ•W‚Æ”¼Œa‚Ì‘å‚«‚³‚ðŽØ‚è‚é
	XMFLOAT3 enemyPos = enemy->GetPosition();
	float enemySize = enemy->GetScale().x;
	bool hit = false;
	for (auto& itr : *bullet)
	{
		if (!itr->GetIsAlive()) { continue; }

		//’eî•ñ
		XMFLOAT3 bulletPos = itr->GetPosition();
		float bulletSize = itr->GetScale().x;

		//“G‚Æ’e‚ªÕ“Ëó‘Ô‚Å‚È‚¢‚È‚ç”²‚¯‚é
		XMFLOAT3 vec = itr->GetVecMove();

		//’e‚©‚ç‚ÌƒŒƒC
		Ray ray;
		ray.start = { bulletPos.x,bulletPos.y,bulletPos.z,0 };
		ray.dir = { vec.x,vec.y,vec.z,0 };

		//“G“–‚½‚è”»’è
		Sphere sphere;
		sphere.center = { enemyPos.x,enemyPos.y,enemyPos.z,0 };
		sphere.radius = enemySize * 10.0f;
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
			}
		}
	}

	return hit;
}
