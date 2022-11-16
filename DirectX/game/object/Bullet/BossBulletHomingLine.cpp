#include "BossBulletHomingLine.h"
#include "BulletManager.h"
#include "GameHelper.h"

using namespace DirectX;

BossBulletHomingLine::BossBulletHomingLine(const XMFLOAT3& _pos, const XMFLOAT3& _color)
{
	pos = _pos;
	move = { 0,0,0 };
	timer = 0;
	color = _color;
}

std::unique_ptr<BossBulletHomingLine> BossBulletHomingLine::Create(const XMFLOAT3& _pos, const XMFLOAT3& _color)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	BossBulletHomingLine* instance = new BossBulletHomingLine(_pos, _color);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<BossBulletHomingLine>(instance);
}

void BossBulletHomingLine::Update()
{
	timer++;

	//�ړ��J�n
	if (timer > 100){
		BulletManager::SetBossBulletHomingLine1(pos, 10.0f, color);
		isAlive = false;
	}

	BaseBullet::Update();
}