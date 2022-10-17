#pragma once
#include "EnemyA.h"

class EnemyManager
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;

public:

	EnemyManager() {};
	~EnemyManager();

	/// <summary>
	/// ����
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<EnemyManager> Create();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	/// <param name="_pos">�v���C���[���W</param>
	void Update(const XMFLOAT3& _pos);

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Draw();

	/// <summary>
	/// ������
	/// </summary>
	void Reset();

	/// <summary>
	/// �e�ƒn�`�A�v���C���[�Ƃ̓����蔻��
	/// </summary>
	/// <param name="_player">�v���C���[���W</param>
	bool CheckCollision(const XMFLOAT3& _player);

private:

	//�^�C�}�[
	int timer;
	//�GA
	static std::vector<std::unique_ptr<EnemyA>> enemyA;
	//�o���ʒu
	static const std::array<XMFLOAT3, 4> popPos;
	//�GA�����t���[���œ|���ꂽ���ǂ���
	bool isKillEnemyA;

public:

	std::vector<std::unique_ptr<EnemyA>>& GetEnemyA() {
		return enemyA;
	}

	bool GetKillBoss() {
		return isKillEnemyA;
	}
};

