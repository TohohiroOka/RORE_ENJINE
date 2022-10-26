#include "BossHpUi.h"
#include "GameHelper.h"
#include "WindowApp.h"

using namespace DirectX;

BossHpUi::BossHpUi(const int _maxHp)
{ 
	//�Q�[�W�̍ő�T�C�Y
	gaugeSizeMax = spriteName[SPRITE::BOSS_HP_OUT].texSize;
	//�T�C�Y�ϊ����̊���
	gaugeRatioX = gaugeSizeMax.x / float(_maxHp);
	//���ۂ̃Q�[�W�T�C�Y
	bossHPGaugeSizeMax = { _maxHp * gaugeRatioX, gaugeSizeMax.y };

	//�E�B���h�E�̕�
	float windowWidth = float(WindowApp::GetWindowWidth());

	//�o�[�O�g
	float difference = 3.0f;
	sprite[0] = Sprite::Create(spriteName[SPRITE::BOSS_HP_OUT].name);
	sprite[0]->SetAnchorpoint({ 0,0 });
	sprite[0]->SetPosition({ windowWidth / 2.0f - gaugeSizeMax.x / 2.0f - difference, 30.0f });
	sprite[0]->SetTexSize(spriteName[SPRITE::BOSS_HP_OUT].texSize);
	sprite[0]->SetSize({ 700, 60.0f });
	sprite[0]->Update();
	//�o�[���g
	sprite[1] = Sprite::Create(spriteName[SPRITE::BOSS_HP_IN].name);
	sprite[1]->SetAnchorpoint({ 0,0 });
	sprite[1]->SetPosition({ windowWidth / 2.0f - gaugeSizeMax.x / 2, 95.0f });
	sprite[1]->SetTexSize(spriteName[SPRITE::BOSS_HP_IN].texSize);
	sprite[1]->SetSize({ 694,20 });
	sprite[1]->Update();
}

std::unique_ptr<BossHpUi> BossHpUi::Create(const int _maxHp)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	BossHpUi* instance = new BossHpUi(_maxHp);
	if (instance == nullptr) {
		return nullptr;
	}

	return std::unique_ptr<BossHpUi>(instance);
}

void BossHpUi::Update(const int _bossHp)
{
	bossHPGaugeSizeMax.x = float(_bossHp) * gaugeRatioX;
	sprite[1]->SetPosition({ float(WindowApp::GetWindowWidth()) / 2.0f - gaugeSizeMax.x / 2, 95.0f });
	sprite[1]->SetSize({ bossHPGaugeSizeMax.x, 20.0f });
	sprite[1]->Update();
}

void BossHpUi::Draw()
{
	for (auto& i : sprite)
	{
		i->Draw();
	}
}
