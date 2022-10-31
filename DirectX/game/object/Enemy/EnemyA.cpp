#include "EnemyA.h"
#include "BulletManager.h"
#include "DirectInput.h"
#include "GameHelper.h"

using namespace DirectX;

EnemyA::EnemyA(const XMFLOAT3& _pos)
{
	//�^�C�}�[
	timer = 0;
	//HP
	hp = 10;
	//�n�ʂɂ��Ă��邩
	onGround = false;
	//���x
	speed = 5.0f;
	//��]�p
	angle = 0;
	//�I�u�W�F�N�g�̐���
	object = Object3d::Create(enemyAModel.get());
	//���W�Z�b�g
	pos = _pos;
	object->SetPosition(pos);

	//�傫���Z�b�g
	scale = 10.0f;
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

	//�p�x
	XMFLOAT2 objAngle = {};
	objAngle.x = GetAngle({ pos.x,pos.z }, { playerPos.x, playerPos.z });//y��
	objAngle.y = GetAngle({ pos.x,pos.y }, { playerPos.x, playerPos.y });//z��
	object->SetRotation({ 0,objAngle.x, objAngle.y });

	//�e�̔���
	if (timer % 50 == 1)
	{
		BulletManager::SetEnemyBullet(pos, 10.0f, { 0.8f,0.2f, 0.8f });
	}

	//�ړ�
	if (pos.y < 600)
	{
		moveVec.y = 0.5f;
	}
	else {
		moveVec.y = 0.0f;
	}

	angle += 1.3f;
	float radiun = XMConvertToRadians(angle);
	moveVec.x = cosf(radiun);
	moveVec.z = sinf(radiun);

	BaseEnemy::Update();
}