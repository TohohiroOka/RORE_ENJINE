#include "UiManager.h"
#include "GameHelper.h"

using namespace DirectX;

std::unique_ptr<UiManager> UiManager::Create(const int _bossMaxHp)
{
	// 3Dオブジェクトのインスタンスを生成
	UiManager* instance = new UiManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize(_bossMaxHp);

	return std::unique_ptr<UiManager>(instance);
}

void UiManager::Initialize(const int _bossMaxHp)
{
	BaseUi::StaticInitialize();
	bossHp = BossHpUi::Create(_bossMaxHp);
}

void UiManager::Update(const int _bossHp)
{
	bossHp->Update(_bossHp);
}

void UiManager::Draw()
{
	bossHp->Draw();
}