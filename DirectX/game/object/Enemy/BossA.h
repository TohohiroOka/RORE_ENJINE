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
		FIREWORKE,//�ԉΓI�Ȃ���
		//BAEM,//�r�[��
		HOMING,//�z�[�~���O
		BOMB_HOMING,//���j���Ă���Ǐ]
		SNAKE,//���˂��˓���
		HOMING_LINE,//�Ǐ]��
		NUM,//���v
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

private:

	//�^�C�}�[
	int timer;
	//�n�ʂɂ��Ă��邩
	bool onGround;
	//���ɏo���e�̗�
	const int bulletNum = 36;
	//xz���ʊp�x
	float angleXZ;
	//�e�̎��
	std::array<int, kindNum> kind;
	//1�O�t���[���̒e�̎��
	std::array<int, kindNum> oldKind;
	//�r�[��
	std::unique_ptr<BossBeam> baem;
	//HOMING_LINE�p�̍��W
	std::array<XMFLOAT3, kindNum> HOMING_LINEpos;
};
