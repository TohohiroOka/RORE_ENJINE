#include "BulletManager.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

std::vector<std::unique_ptr<FixedTurretBullet>> BulletManager::fixedTurretBullet;
std::vector<std::unique_ptr<EnemyABullet>> BulletManager::enemyABullet;

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

	//��~�����e�𐏎�����
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
}

void BulletManager::Reset()
{
	fixedTurretBullet.clear();
}

bool BulletManager::CheckCollision(const XMFLOAT3& _pos)
{
	//�Փ˗p�ɍ��W�Ɣ��a�̑傫�����؂��
	XMFLOAT3 playerPos = _pos;
	playerPos.y += 10.0f;
	float playerSize = 5;

	bool isHit = false;

	//�e�ƃv���C���[���Փˏ�ԂłȂ��Ȃ甲����
	for (auto& i : fixedTurretBullet)
	{
		if (!i->GetIsAlive()) { continue; }

		//�v���C���[�Ɠ������Ă��������
		if (Collision::CheckCircle2Circle(
			i->GetPosition(), i->GetScale(), playerPos, playerSize)) {
			isHit = true;
			i->SetIsAlive(false);
		}
	}
	for (auto& i : enemyABullet)
	{
		if (!i->GetIsAlive()) { continue; }

		//�v���C���[�Ɠ������Ă��������
		if (Collision::CheckCircle2Circle(
			i->GetPosition(), i->GetScale(), playerPos, playerSize)) {
			isHit = true;
			i->SetIsAlive(false);
		}
	}

	return isHit;
}