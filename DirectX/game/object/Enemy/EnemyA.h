#pragma once
#include "BaseEnemy.h"

class EnemyA : public BaseEnemy
{
public:

	EnemyA(const XMFLOAT3& _pos);
	~EnemyA() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <returns></returns>
	static std::unique_ptr<EnemyA> Create(const XMFLOAT3& _pos);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

private:

	//�^�C�}�[
	int timer;
	//���ɏo���e�̗�
	const int bulletNum = 10;
};

