#pragma once
#include "BaseEnemy.h"

class BossA : public BaseEnemy
{
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

private:

	//�^�C�}�[
	int timer;
	//�n�ʂɂ��Ă��邩
	bool onGround;
	//���ɏo���e�̗�
	const int bulletNum = 10;
};
