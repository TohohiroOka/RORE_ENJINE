#pragma once
#include "TutorialEnemy.h"
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
	static std::list<std::unique_ptr<BaseEnemy>> enemy;
	//�v���C���[���W
	static XMFLOAT3 playerPos;

public:

	static void SetEnemyAMoveList(const XMFLOAT3& _pos, const std::vector<int> _destination) {
		EnemyA::SetMoveList(_pos, _destination);
	}

	static void SetTutorialEnemy(const XMFLOAT3& _pos) {
		enemy.emplace_back(TutorialEnemy::Create(_pos));
	}

	static void SetEnemyA(const XMFLOAT3& _pos, const int _destination) {
		enemy.emplace_back(EnemyA::Create(_pos, _destination));
	}

	std::list<std::unique_ptr<BaseEnemy>>::iterator& GetEnemyA() {
		return enemy.begin();
	}
};