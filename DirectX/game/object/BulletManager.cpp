#include "BulletManager.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

#include "GameHelper.h"

std::list<std::unique_ptr<PlayerBullet>> BulletManager::playerBullet;
DirectX::XMFLOAT3 BulletManager::playerPos;

BulletManager::~BulletManager()
{
	BaseBullet::Finalize();
}

std::unique_ptr<BulletManager> BulletManager::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	BulletManager* instance = new BulletManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<BulletManager>(instance);
}

void BulletManager::Initialize()
{
	playerPos = { 0,0,0 };
	BaseBullet::StaticInitialize();
}

void BulletManager::Update(const XMFLOAT3& _playerPos)
{
	BaseBullet::StaticUpdate();

	playerPos = _playerPos;

	for (auto& i : playerBullet)
	{
		i->Update();
	}

	//��~�����e�𐏎�����
	{
		for (auto it = playerBullet.begin(); it != playerBullet.end();)
		{
			if (!it->get()->GetIsAlive())
			{
				// �폜���ꂽ�v�f�̎����w���C�e���[�^���Ԃ����B
				it = playerBullet.erase(it);
			}
			// �v�f�폜�����Ȃ��ꍇ�ɁA�C�e���[�^��i�߂�
			else {
				++it;
			}
		}
	}
}

void BulletManager::Draw()
{
	BaseBullet::Draw();
}

void BulletManager::EffectDraw()
{
	BaseBullet::EffectDraw();
}

void BulletManager::Reset()
{
}

bool BulletManager::CheckPlayerBulletToEnemyCollision(const XMFLOAT3& _pos, float _scale)
{
	XMFLOAT3 enemyPos = _pos;
	bool hit = false;

	for (auto& itr : playerBullet)
	{
		if (!itr->GetIsAlive()) { continue; }

		//�e���
		XMFLOAT3 bulletPos = itr->GetPosition();
		float bulletSize = itr->GetScale();
		XMFLOAT3 vec = itr->GetMoveVec();

		//�ړ������̃��C���
		Ray ray;
		ray.start = { bulletPos.x,bulletPos.y,bulletPos.z,0 };
		ray.dir = { vec.x,vec.y,vec.z,0 };

		//�G�l�~�[��collider
		Sphere sphere;
		sphere.center = { enemyPos.x,enemyPos.y,enemyPos.z,0 };
		sphere.radius = _scale * 5.0f;
		if (Collision::CheckRay2Sphere(ray, sphere))
		{
			float x = powf(enemyPos.x - bulletPos.x, 2);
			float y = powf(enemyPos.y - bulletPos.y, 2);
			float z = powf(enemyPos.z - bulletPos.z, 2);

			float eneTobul1 = sqrt(x + y + z);
			XMFLOAT3 beforepos = itr->GetMove();
			x = powf(beforepos.x, 2);
			y = powf(beforepos.y, 2);
			z = powf(beforepos.z, 2);

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