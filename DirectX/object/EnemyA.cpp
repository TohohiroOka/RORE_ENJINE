#include "EnemyA.h"
#include "BulletManager.h"

#include "DirectInput.h"

using namespace DirectX;

EnemyA::EnemyA(const XMFLOAT3& _pos)
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
	scale = 3.0f;
	object->SetScale({ scale ,scale ,scale });
}

std::unique_ptr<EnemyA> EnemyA::Create(const XMFLOAT3& _pos)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	EnemyA* instance = new EnemyA(_pos);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<EnemyA>(instance);
}

void EnemyA::Update()
{
	timer++;

	//if (timer % 50 == 1)
	//{
	//	float angle = 360.0f / float(bulletNum);
	//	for (int i = 0; i < bulletNum; i++)
	//	{
	//		float nowAngle = angle * i;
	//		float radiun = XMConvertToRadians(nowAngle);
	//		BulletManager::SetEnemyABullet(pos, { cos(radiun),0.0f,sin(radiun) });
	//	}
	//}

	BaseEnemy::Update();
}