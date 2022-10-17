#include "EnemyManager.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

using namespace DirectX;

std::vector<std::unique_ptr<EnemyA>> EnemyManager::enemyA;
const std::array<XMFLOAT3, 4> EnemyManager::popPos = {
	XMFLOAT3(1905.0f,50.0f,3000.0f),
XMFLOAT3(1905.0f,50.0f,810.0f),
XMFLOAT3(3000.0f,50.0f,1905.0f),
XMFLOAT3(810.0f,50.0f,1905.0f),
};

EnemyManager::~EnemyManager()
{
	//�Œ�C��̒e
	enemyA.clear();
	std::vector<std::unique_ptr<EnemyA>>().swap(enemyA);

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
	timer = 0;
	enemyA.emplace_back(EnemyA::Create(popPos[1]));

	isKillEnemyA = false;
}

void EnemyManager::Update(const XMFLOAT3& _pos)
{
	timer++;
	isKillEnemyA = false;

	//�G�ǉ�
	if (timer % 100 == 1 && enemyA.size() < 20)
	{
		int randN = rand() % 4;
		enemyA.emplace_back(EnemyA::Create(popPos[randN]));
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
			isKillEnemyA = true;
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

	//�v���C���[���Փˏ�ԂłȂ��Ȃ甲����
	for (auto& i : enemyA)
	{
		if (!i->GetIsAlive()) { continue; }

		//�v���C���[�Ɠ������Ă����牟���߂�
		if (Collision::CheckCircle2Circle(
			i->GetPosition(), i->GetScale(), playerPos, playerSize)) {
		}
	}

	return isHit;
}