#include "BulletManager.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

#include "GameHelper.h"

std::list<std::unique_ptr<PlayerBullet>> BulletManager::playerBullet;
std::list<std::unique_ptr<BaseBullet>> BulletManager::bossBullet;
DirectX::XMFLOAT3 BulletManager::playerPos;

BulletManager::~BulletManager()
{
	//�Œ�C��̒e
	bossBullet.clear();
	std::list<std::unique_ptr<BaseBullet>>().swap(bossBullet);

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
	playerPos = _playerPos;

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

	//��~�����e�𐏎�����
	{
		for (auto it = bossBullet.begin(); it != bossBullet.end();)
		{
			if (!it->get()->GetIsAlive())
			{
				// �폜���ꂽ�v�f�̎����w���C�e���[�^���Ԃ����B
				it = bossBullet.erase(it);
			}
			// �v�f�폜�����Ȃ��ꍇ�ɁA�C�e���[�^��i�߂�
			else {
				++it;
			}
		}
		const int p_size = int(playerBullet.size());
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

bool BulletManager::CheckEnemyBulletToPlayerCollision()
{
	//�Փ˗p�ɍ��W�Ɣ��a�̑傫�����؂��
	playerPos.y += 10.0f;
	float playerSize = 5;

	bool isHit = false;

	//�e�ƃv���C���[���Փˏ�ԂłȂ��Ȃ甲����
	for (auto& i : bossBullet)
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

void BulletManager::SetBossBulletFF(const XMFLOAT3& _pos, const float _speed, const XMFLOAT3& _color)
{
	XMFLOAT3 rand = { Randomfloat(300) - 150.0f,Randomfloat(300) - 150.0f ,Randomfloat(300) - 150.0f };
	Vector3 moveVec = { rand.x + playerPos.x - _pos.x,rand.y + playerPos.y - _pos.y ,rand.z + playerPos.z - _pos.z };
	moveVec = moveVec.normalize();
	bossBullet.emplace_back(BossBulletNormal::Create(_pos, { moveVec.x,moveVec.y,moveVec.z }, _speed, _color));
}