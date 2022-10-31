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
	/// <param name="_player">�v���C���[���W</param>
	void Update(const XMFLOAT3& _playerPos);

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
	bool CheckCollision();

private:

	//�GA
	static std::list<std::unique_ptr<EnemyA>> enemyA;
	//�v���C���[���W
	static XMFLOAT3 playerPos;

public:

	static void SetEnemyA(const XMFLOAT3& _pos) {
		enemyA.emplace_back(EnemyA::Create(_pos));
	}

	std::list<std::unique_ptr<EnemyA>>::iterator& GetEnemyA() {
		return enemyA.begin();
	}
};

