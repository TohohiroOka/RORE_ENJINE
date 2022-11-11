#pragma once
#include "BaseUi.h"

class PlayerUi : public BaseUi
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_maxHp">�v���C���[�ő�HP</param>
	PlayerUi(const int _maxHp);
	~PlayerUi() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_maxHp">�v���C���[�ő�HP</param>
	/// <returns></returns>
	static std::unique_ptr<PlayerUi> Create(const int _maxHp);

	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="_playerHp">�v���C���[hp</param>
	void Update(const int _playerHp);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

private:

	//�v���C���[�̍ő�hp
	int maxHp;
	//�X�v���C�g
	std::unique_ptr<Sprite> damage;
	//�_���[�W�X�v���C�g�̃��l
	float damageAlpha;
};
