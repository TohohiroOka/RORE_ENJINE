#include "TutorialEnemy.h"

using namespace DirectX;

TutorialEnemy::TutorialEnemy(const XMFLOAT3& _pos)
{
	//HP
	hp = 10;
	//�n�ʂɂ��Ă��邩
	onGround = false;
	//���x
	speed = 0.0f;
	//�I�u�W�F�N�g�̐���
	object = Object3d::Create(enemyAModel.get());
	//���W�Z�b�g
	pos = _pos;
	object->SetPosition(pos);

	//�傫���Z�b�g
	scale = 10.0f;
	object->SetScale({ scale ,scale ,scale });

	moveVec = { 0,0,0 };
}

std::unique_ptr<TutorialEnemy> TutorialEnemy::Create(const XMFLOAT3& _pos)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	TutorialEnemy* instance = new TutorialEnemy(_pos);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<TutorialEnemy>(instance);
}