#pragma once
#include "BaseEnemy.h"

class EnemyA : public BaseEnemy
{
public:

	EnemyA(const XMFLOAT3& _pos, const int _destination);
	~EnemyA() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_destination">���̈ړ���ԍ�</param>
	/// <returns></returns>
	static std::unique_ptr<EnemyA> Create(const XMFLOAT3& _pos, const int _destination);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

private:

	//�ړ��o�H
	static std::vector<MOVE_LIST> moveList;

private:

	//�^�C�}�[
	int timer;
	//��]�p
	float angle;
	//�ړ��t���O
	bool isMove;
	//�ړ��^�C�}�[
	int moveTimer;
	//���݂̈ړ���ԍ�
	int destinationNumber;

public:
	static void SetMoveList(const XMFLOAT3& _pos, const std::vector<int> _destination) {
		MOVE_LIST add;
		add.pos = _pos;
		add.destination = _destination;
		moveList.emplace_back(add);
	}

};