#include "EnemyManager.h"
#include "FirstBoss.h"
#include "Player.h"


std::unique_ptr<EnemyManager> EnemyManager::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	EnemyManager* instance = new EnemyManager();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->Initialize();

	return std::unique_ptr<EnemyManager>(instance);
}

void EnemyManager::Initialize()
{
	enemy = FirstBoss::Create();
}

void EnemyManager::Update()
{
	enemy->Update();
}

void EnemyManager::Draw()
{
	enemy->Draw();
}

void EnemyManager::SetTarget()
{
	Player::SetTarget(enemy->GetPosition());
}
