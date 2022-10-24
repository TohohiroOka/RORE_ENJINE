#pragma once
#include "BossHpUi.h"

class UiManager
{
public:

	UiManager() {};
	~UiManager() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_bossMaxHp">�{�X�ő�HP</param>
	/// <returns></returns>
	static std::unique_ptr<UiManager> Create(const int _bossMaxHp);

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_bossMaxHp">�{�X�ő�HP</param>
	void Initialize(const int _bossMaxHp);

	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="_bossHp">�{�X�ő�HP</param>
	void Update(const int _bossHp);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

private:

	std::unique_ptr<BossHpUi> bossHp;
};