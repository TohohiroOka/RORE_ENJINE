#pragma once
#include "BaseEnemy.h"
#include "BossBeam.h"

class BossA : public BaseEnemy
{
private:

	enum class BULLET_KIND
	{
		CIRCLE,//�~��ɏo��
		FIREWORKE,//�ԉΓI�Ȃ���
		BAEM,//�r�[��
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
	int kind = int(BULLET_KIND::CIRCLE);
	//�r�[��
	std::unique_ptr<BossBeam> baem;
};
