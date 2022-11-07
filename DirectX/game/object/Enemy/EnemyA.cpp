#include "EnemyA.h"
#include "BulletManager.h"
#include "GameHelper.h"
#include "Easing.h"

using namespace DirectX;

std::vector<BaseEnemy::MOVE_LIST> EnemyA::moveList;

EnemyA::EnemyA(const XMFLOAT3& _pos, const int _destination)
{
	//�^�C�}�[
	timer = 0;
	//HP
	hp = 10;
	//�n�ʂɂ��Ă��邩
	onGround = false;
	//���x
	speed = 3.0f;
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

	destinationNumber = _destination;
	moveVec = { 0,0,0 };
	isMove = true;
}

std::unique_ptr<EnemyA> EnemyA::Create(const XMFLOAT3& _pos, const int _destination)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	EnemyA* instance = new EnemyA(_pos, _destination);
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
	const int maxTimer = 50;
	//�p�x
	XMFLOAT2 objAngle = {};
	objAngle.x = GetAngle({ pos.x,pos.z }, { playerPos.x, playerPos.z });//y��
	objAngle.y = GetAngle({ pos.x,pos.y }, { playerPos.x, playerPos.y });//z��
	object->SetRotation({ 0,objAngle.x, objAngle.y });

	//�e�̔���
	if (timer % 150 == 0)
	{
		BulletManager::SetEnemyBullet(pos, 10.0f, { 0.8f,0.2f, 0.8f });
	}

	if (isMove)
	{
		moveTimer++;
		float ratio = (float)moveTimer / (float)maxTimer;
		int nextNum = moveList[destinationNumber].destination[0];
		pos.x = Easing::Lerp(moveList[destinationNumber].pos.x, moveList[nextNum].pos.x, ratio);
		pos.z = Easing::Lerp(moveList[destinationNumber].pos.z, moveList[nextNum].pos.z, ratio);

		if (ratio >= 1.0f) {
			destinationNumber = moveList[destinationNumber].destination[0];
			moveTimer = 0;
		}
	}

	BaseEnemy::Update();
}