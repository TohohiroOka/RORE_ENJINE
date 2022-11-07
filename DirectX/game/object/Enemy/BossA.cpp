#include "BossA.h"
#include "BulletManager.h"
#include "DebugText.h"
#include "GameHelper.h"
#include "EnemyManager.h"
#include "Easing.h"

using namespace DirectX;

std::vector<BaseEnemy::MOVE_LIST> BossA::moveList;

BossA::BossA(const XMFLOAT3& _pos, const int _destination)
{
	BossBeam::StaticInitialize();
	//�^�C�}�[
	timer = 0;
	//HP
	hp = maxHp;
	//�n�ʂɂ��Ă��邩
	onGround = false;
	//�ړ����x
	speed = 1.0f;
	//�I�u�W�F�N�g�̐���
	object = Object3d::Create(bossModel.get());
	//���W�Z�b�g
	pos = _pos;
	pos.y = 200.0f;
	object->SetPosition(pos);

	//�傫���Z�b�g
	scale = 30.0f;
	object->SetScale({ scale ,scale ,scale });

	//�u���[����
	object->SetBloom(true);
	object->SetOutline(true);

	//�ړ��t���O
	isMove = true;
	//�ړ��^�C�}�[
	moveTimer = 0;
	//���݂̈ړ���ԍ�
	destinationNumber = _destination;
	//���̈ړ���ԍ�
	int rand = (int)Randomfloat((int)moveList[destinationNumber].destination.size());
	nextDestinationNumber = rand;

	//�G�����J�E���g
	summonEnemyCount = 0;

	//�r�[��
	baem = BossBeam::Create();

	//�U��������
	attack[0].kind = int(BULLET_KIND::CIRCLE);
	attack[1].kind = int(BULLET_KIND::BOMB_HOMING);
	for (auto& i : attack)
	{
		i.oldKind = i.kind;
	}
}

std::unique_ptr<BossA> BossA::Create(const XMFLOAT3& _pos, const int _destination)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	BossA* instance = new BossA(_pos, _destination);
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

	//if (DirectInput::GetInstance()->TriggerKey(DIK_K))
	//{
	//	attack[0].kind++;
	//	attack[0].kind = attack[0].kind % int(BULLET_KIND::SIZE);
	//}

	//�U���̍X�V
	if (summonEnemyCount <= 5)
	{
		if (timer % 300 == 1)
		{
			for (auto& i : attack)
			{
				i.kind++;
				i.kind = i.kind % int(BULLET_KIND::SIZE);
				i.rota = { 0,0,0 };
			}
			summonEnemyCount++;
		}

		//�U��
		Attack();
	}

	//�ړ�
	if (isMove)
	{
		moveTimer++;
		const int maxTimer = 150;
		const float ratio = (float)moveTimer / (float)maxTimer;
		pos.x = Easing::Lerp(moveList[destinationNumber].pos.x, moveList[nextDestinationNumber].pos.x, ratio);
		pos.z = Easing::Lerp(moveList[destinationNumber].pos.z, moveList[nextDestinationNumber].pos.z, ratio);

		if (ratio >= 1.0f) {
			int rand = 0;
			while (rand == destinationNumber) {
				rand = (int)Randomfloat((int)moveList[destinationNumber].destination.size());
			}
			destinationNumber = nextDestinationNumber;
			nextDestinationNumber = moveList[destinationNumber].destination[rand];
			moveTimer = 0;
		}
	}


	BaseEnemy::Update();

	DebugText* text = DebugText::GetInstance();
	std::string bossHp = std::to_string(hp);
	std::string bossAttack1= std::to_string(attack[0].kind);
	std::string bossAttack2 = std::to_string(attack[1].kind);
	std::string stdSummonEnemyCount = std::to_string(summonEnemyCount);
	text->Print("Boss hp : " + bossHp, 100, 200);
	text->Print("Boss Attack1 : " + bossAttack1+ " Boss Attack2 : " + bossAttack2, 100, 225);
	text->Print("summonEnemyCount : " + stdSummonEnemyCount, 100, 250);

	text = nullptr;
}

void BossA::Draw()
{
	BaseEnemy::Draw();

	if (!baem->GetIsAlive()) { return; }
	//baem->Draw();
}

void BossA::Attack()
{
	//for (int i = 0; i < 1; i++)
	for (int i = 0; i < kindNum; i++)
	{
		if (attack[i].kind == int(BULLET_KIND::CIRCLE) && timer % 10 == 1)
		{
			float _angleXZ = 5.0f;
			for (int j = 0; j < 36; j++)
			{
				_angleXZ += 10.0f;
				float radiunXZ = XMConvertToRadians(_angleXZ);
				float cosXZ = cosf(radiunXZ);
				float sonXZ = sinf(radiunXZ);

				for (int i = 0; i < 8; i++)
				{
					float ratio = float(i) / 8.0f;
					float nowAngle = ratio * 360.0f;
					float radiun = XMConvertToRadians(nowAngle);

					BulletManager::SetBossBulletA(pos,
						{ cos(radiun) * cosXZ,cos(radiun) * sonXZ,sin(radiun) });
				}
			}
		}
		else if (attack[i].kind == int(BULLET_KIND::CIRCLE_ROTATE))
		{
			if (timer % 3 == 1)
			{
				attack[i].rota.y += 10.0f;
				float radiunXZ = XMConvertToRadians(attack[i].rota.y);
				for (int i = 0; i < bulletNum; i++)
				{
					float ratio = float(i) / float(bulletNum);
					float nowAngle = ratio * 360.0f;
					float radiun = XMConvertToRadians(nowAngle);
					BulletManager::SetBossBulletA(pos,
						{ cos(radiun) * cos(radiunXZ),cos(radiun) * sin(radiunXZ),sin(radiun) });
				}
			}
		}
		//else if (kind[i] == int(BULLET_KIND::BAEM))
		//{
		//	baem->Update(pos, { 0.9f,0.0f,0.9f });
		//}
		else if (attack[i].kind == int(BULLET_KIND::HOMING))
		{
			XMFLOAT3 color = { Randomfloat(100) / 100.0f,Randomfloat(100) / 100.0f, Randomfloat(100) / 100.0f, };
			BulletManager::SetBossBulletC(pos, color);
		}
		else if (attack[i].kind == int(BULLET_KIND::BOMB_HOMING))
		{
			if (timer % 3 == 1)
			{
				XMFLOAT2 _angle = { Randomfloat(360),Randomfloat(360) };
				XMFLOAT2 radiun = { XMConvertToRadians(_angle.x),XMConvertToRadians(_angle.y) };

				XMFLOAT3 color = { Randomfloat(100) / 100.0f,Randomfloat(100) / 100.0f, Randomfloat(100) / 100.0f, };
				BulletManager::SetBossBulletF(pos, { cos(radiun.x) * cos(radiun.y),cos(radiun.x) * sin(radiun.y),sin(radiun.x) }, color);
			}
		}
		else if (attack[i].kind == int(BULLET_KIND::SNAKE))
		{
			XMFLOAT3 color = { Randomfloat(100) / 100.0f,Randomfloat(100) / 100.0f, Randomfloat(100) / 100.0f, };
			BulletManager::SetBossBulletD(pos, color);
		}
		else if (attack[i].kind == int(BULLET_KIND::HOMING_LINE1))
		{
			for (int lineNumber = 0; lineNumber < homing_line_num; lineNumber++)
			{
				//����̊p�x
				const float angleOne = 360 / homing_line_num;

				//�O�������W�̃Z�b�g
				if (attack[i].oldKind != int(BULLET_KIND::HOMING_LINE1))
				{
					attack[i].HOMING_LINEpos[lineNumber] = { pos.x + Randomfloat(100) - 50.0f,
						pos.y + Randomfloat(100) - 50.0f,pos.z + Randomfloat(100) - 50.0f };

					attack[i].radiun[lineNumber] = { XMConvertToRadians(Randomfloat(360 * 2) - 360.0f),
						XMConvertToRadians(i * angleOne) };
				}

				//�e�Z�b�g
				XMFLOAT3 color = { Randomfloat(100) / 100.0f,Randomfloat(100) / 100.0f, Randomfloat(100) / 100.0f, };
				BulletManager::SetBossBulletE(attack[i].HOMING_LINEpos[lineNumber], 3.0f, color, false);

				const float speed = 2.0f;
				attack[i].HOMING_LINEpos[lineNumber].x += speed * cos(attack[i].radiun[lineNumber].x) * cos(attack[i].radiun[lineNumber].y);
				attack[i].HOMING_LINEpos[lineNumber].y += speed * cos(attack[i].radiun[lineNumber].x) * sin(attack[i].radiun[lineNumber].y);
				attack[i].HOMING_LINEpos[lineNumber].z += speed * sin(attack[i].radiun[lineNumber].x);

				//�^�C�}�[�ȏ�ŒǏ]���[�h�ɕω�
				if (timer / 300 > 30)
				{
					attack[i].kind = int(BULLET_KIND::HOMING_LINE2);
				}
			}
		}
		else if (attack[i].kind == int(BULLET_KIND::HOMING_LINE2))
		{
			for (auto& linePos : attack[i].HOMING_LINEpos)
			{
				XMFLOAT3 color = { Randomfloat(100) / 100.0f,Randomfloat(100) / 100.0f, Randomfloat(100) / 100.0f, };
				BulletManager::SetBossBulletE(linePos, 3.0f, color,1);
			}
		}

		//if (kind[i] != int(BULLET_KIND::BAEM))
		//{
		//	baem->SetIsAlive(false);
		//}

		//�O�t���[���̏W�ނ�ۑ�
		attack[i].oldKind = attack[i].kind;
	}
}
