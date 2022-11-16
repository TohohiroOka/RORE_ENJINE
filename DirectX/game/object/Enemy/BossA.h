#pragma once
#include "BaseEnemy.h"
#include <array>

class BossA : public BaseEnemy
{
private:

	enum class BULLET_KIND
	{
		CIRCLE,//�~��ɏo��
		LATTICE_BAEM_SET_X,//�i�q��r�[���Z�b�gyz����
		LATTICE_BAEM_SET_Y,//�i�q��r�[���Z�b�gxz����
		LATTICE_BAEM_SET_Z,//�i�q��r�[���Z�b�gxy����
		FIREWORKE,//�ԉΓI�Ȃ���
		HOMING,//�z�[�~���O
		CIRCLE_ROTATE,//�~��ɏo���]����
		BOMB_HOMING,//���j���Ă���Ǐ]
		SNAKE,//���˂��˓���
		HOMING_LINE1,//�Ǐ]��(�~�`�ɏo��)
		HOMING_LINE2,//�Ǐ]��(�Ǐ]����)
		SHOCK_WAVE,//�Ռ��g
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
		//�r�[���̃Z�b�g��
		int beamSetNum = 0;
		//�i�q�r�[���̍��W
		std::array<int,2> lattice_beam_pos = { 0,0 };
	};

	static const int lattice_beam_side_num = 15;

public:

	BossA(const XMFLOAT3& _pos, const int _destination);
	~BossA() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_destination">���̈ړ���ԍ�</param>
	/// <returns></returns>
	static std::unique_ptr<BossA> Create(const XMFLOAT3& _pos, const int _destination);

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
	static const int kindNum = 1;
	//�{�X�̍ő�̗�
	const int maxHp = 400;
	//�ړ��o�H
	static std::vector<MOVE_LIST> moveList;
	//parts�̐�
	static const int partsNum = 8;
	//���S����̋���
	static const std::array<XMFLOAT3, partsNum> partsPos;

private:

	//����̃I�u�W�F�N�g
	std::array<std::unique_ptr<Object3d>, partsNum> parts;
	//�^�C�}�[
	int timer;
	//�n�ʂɂ��Ă��邩
	bool onGround;
	//�ړ��t���O
	bool isMove;
	//�ړ��^�C�}�[
	int moveTimer;
	//���݂̈ړ���ԍ�
	int destinationNumber;
	//���̈ړ���ԍ�
	int nextDestinationNumber;
	//���ɏo���e�̗�
	const int bulletNum = 36;
	//��񕪂̍U�����
	std::array<BULLET_INFO, kindNum> attack;

public:

	int GetMaxHp() { return maxHp; }
	int GetHp() { return hp; }
	static void SetMoveList(const XMFLOAT3& _pos, const std::vector<int> _destination) {
		MOVE_LIST add;
		add.pos = _pos;
		add.destination = _destination;
		moveList.emplace_back(add);
	}

};
