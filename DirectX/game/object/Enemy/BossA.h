#pragma once
#include "BaseEnemy.h"
#include "BossBeam.h"
#include <array>

class BossA : public BaseEnemy
{
private:

	enum class BULLET_KIND
	{
		CIRCLE,//�~��ɏo��
		CIRCLE_ROTATE,//�~��ɏo���]����
		CIRCLE_UP_DOWN,//�~��ɏ㉺����
		FIREWORKE,//�ԉΓI�Ȃ���
		//BAEM,//�r�[��
		HOMING,//�z�[�~���O
		BOMB_HOMING,//���j���Ă���Ǐ]
		SNAKE,//���˂��˓���
		HOMING_LINE1,//�Ǐ]��(�~�`�ɏo��)
		HOMING_LINE2,//�Ǐ]��(�Ǐ]����)
		SIZE,//���v
	};

	static const int homing_line_num = 6;

	//��񕪂̍U�����
	struct BULLET_INFO
	{
		//�e�̎��
		int kind;
		//1�O�t���[���̒e�̎��
		int oldKind;
		//HOMING_LINE�p�̍��W
		XMFLOAT3 HOMING_LINEpos[homing_line_num];
		//�p�x�z�[�~���O���p�p�x
		XMFLOAT2 radiun[homing_line_num];
		//��]��
		XMFLOAT3 rota = { 0,0,0 };
	};

public:

	BossA(const XMFLOAT3& _pos);
	~BossA() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <returns></returns>
	static std::unique_ptr<BossA> Create(const XMFLOAT3& _pos);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Draw() override;

	/// <summary>
	/// �U��
	/// </summary>
	void Attack();

private:

	//���ɂł���U���̎��
	static const int kindNum = 2;
	//�{�X�̍ő�̗�
	const int maxHp = 400;
	//�ЉH�̑���
	static const int wing_num = 5;

private:

	//�^�C�}�[
	int timer;
	//�n�ʂɂ��Ă��邩
	bool onGround;
	//���ɏo���e�̗�
	const int bulletNum = 36;
	//��񕪂̍U�����
	std::array<BULLET_INFO, kindNum> attack;
	//�G����������J�E���g
	int summonEnemyCount;
	//�r�[��
	std::unique_ptr<BossBeam> baem;

public:

	int GetMaxHp() { return maxHp; }
	int GetHp() { return hp; }
};
