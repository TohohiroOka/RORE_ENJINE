#include "BossA.h"
#include "BulletManager.h"
#include "DebugText.h"

using namespace DirectX;

BossA::BossA(const XMFLOAT3& _pos)
{
	//�^�C�}�[
	timer = 0;
	//HP
	hp = 10;
	//�n�ʂɂ��Ă��邩
	onGround = false;
	//�ړ����x
	speed = 1.0f;
	//�I�u�W�F�N�g�̐���
	object = Object3d::Create(model.get());
	//���W�Z�b�g
	pos = _pos;
	object->SetPosition(pos);

	//�傫���Z�b�g
	scale = 10.0f;
	object->SetScale({ scale ,scale ,scale });
}

std::unique_ptr<BossA> BossA::Create(const XMFLOAT3& _pos)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	BossA* instance = new BossA(_pos);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<BossA>(instance);
}

void BossA::Update()
{
	timer++;

	if (timer % 5 == 1)
	{
		angleXZ += 10.0f;
		float radiunXZ = XMConvertToRadians(angleXZ);
		for (int i = 0; i < bulletNum; i++)
		{
			float ratio = float(i) / float(bulletNum);
			float nowAngle = ratio * 360.0f;
			float radiun = XMConvertToRadians(nowAngle);
			XMFLOAT3 color = {};
			if (int(angleXZ) % 3 == 0)
			{
				color = { 0.9f,0.2f,0.2f };
			}
			else if (int(angleXZ) % 3 == 1)
			{
				color = { 0.2f,0.9f,0.2f };
			}
			else
			{
				color = { 0.2f,0.2f,0.9f };
			}

			BulletManager::SetEnemyABullet(pos,
				{ cos(radiun) * cos(radiunXZ),cos(radiun) * sin(radiunXZ),sin(radiun) }, color);
		}
	}

	BaseEnemy::Update();

	DebugText* text = DebugText::GetInstance();
	std::string bossHp = std::to_string(hp);
	text->Print("Boss hp : " + bossHp, 100, 200);
	text = nullptr;
}