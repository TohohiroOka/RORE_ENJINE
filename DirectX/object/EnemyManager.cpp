#include "EnemyManager.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

std::vector<std::unique_ptr<EnemyA>> EnemyManager::enemyA;

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
	timer = 0;
}

void EnemyManager::Update(const XMFLOAT3& _pos)
{
	timer++;

	//�G�ǉ�
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

	//��~�����e�𐏎�����
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
	//�Փ˗p�ɍ��W�Ɣ��a�̑傫�����؂��
	XMFLOAT3 playerPos = _pos;
	playerPos.y += 10.0f;
	float playerSize = 5;

	bool isHit = false;

	//�e�ƃv���C���[���Փˏ�ԂłȂ��Ȃ甲����
	for (auto& i : enemyA)
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