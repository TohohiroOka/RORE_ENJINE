#include "GameCollision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

using namespace DirectX;

bool GameCollision::CheckBulletToEnemy(BaseBullet* _bullet, BaseEnemy* _enemy)
{
	//�G���
	XMFLOAT3 ePos = _enemy->GetPosition();

	//�e���
	XMFLOAT3 bPos = _bullet->GetPosition();
	XMFLOAT3 bVec = _bullet->GetMoveVec();

	//�e����̃��C
	Ray ray;
	ray.start = { bPos.x,bPos.y,bPos.z,0 };
	ray.dir = { bVec.x,bVec.y,bVec.z,0 };

	//�G�����蔻��
	Sphere sphere;
	sphere.center = { ePos.x,ePos.y,ePos.z,0 };
	sphere.radius = _enemy->GetScale() / 2.0f;

	//�Փ˔���
	if (!Collision::CheckRay2Sphere(ray, sphere)) { return false; }

	//�G�ƒe�̋���
	float x = powf(ePos.x - bPos.x, 2);
	float y = powf(ePos.y - bPos.y, 2);
	float z = powf(ePos.z - bPos.z, 2);
	float eneTobul = sqrt(x + y + z);

	//�e�̂P�t���[����̂܂ł̋���
	XMFLOAT3 bMove = _bullet->GetMove();
	x = powf(bMove.x, 2);
	y = powf(bMove.y, 2);
	z = powf(bMove.z, 2);
	float bBulTobul = sqrt(x + y + z);

	if (!(eneTobul < bBulTobul)) { return false; }

	return true;
}
