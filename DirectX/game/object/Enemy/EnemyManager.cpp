#include "EnemyManager.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "GameHelper.h"

using namespace DirectX;

std::list<std::unique_ptr<EnemyA>> EnemyManager::enemyA;
XMFLOAT3 EnemyManager::playerPos;

EnemyManager::~EnemyManager()
{
	//�Œ�C��̒e
	enemyA.clear();
	std::list<std::unique_ptr<EnemyA>>().swap(enemyA);

	BaseEnemy::Finalize();
}

std::unique_ptr<EnemyManager> EnemyManager::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	EnemyManager* instance = new EnemyManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<EnemyManager>(instance);
}

void EnemyManager::Initialize()
{
	BaseEnemy::StaticInitialize();
}

void EnemyManager::Update(const XMFLOAT3& _playerPos)
{
	playerPos = _playerPos;

	BaseEnemy::StaticUpdate(playerPos);

	for (auto& i : enemyA)
	{
		i->Update();
	}

	//�|���ꂽ�G�𐏎�����
	for (auto it = enemyA.begin(); it != enemyA.end();)
	{
		if (!it->get()->GetIsAlive())
		{
			// �폜���ꂽ�v�f�̎����w���C�e���[�^���Ԃ����B
			it = enemyA.erase(it);
		}
		// �v�f�폜�����Ȃ��ꍇ�ɁA�C�e���[�^��i�߂�
		else {
			++it;
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

bool EnemyManager::CheckCollision()
{
	//�Փ˗p�ɍ��W�Ɣ��a�̑傫�����؂��
	XMFLOAT3 _playerPos = playerPos;
	_playerPos.y += 10.0f;
	float playerSize = 5;

	bool isHit = false;

	//�v���C���[���Փˏ�ԂłȂ��Ȃ甲����
	for (auto& i : enemyA)
	{
		if (!i->GetIsAlive()) { continue; }

		//�v���C���[�Ɠ������Ă����牟���߂�
		if (Collision::CheckCircle2Circle(
			i->GetPosition(), i->GetScale(), _playerPos, playerSize)) {
		}
	}

	return isHit;
}