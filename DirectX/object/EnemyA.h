#pragma once
#include "BaseEnemy.h"

class EnemyA : public BaseEnemy
{
public:

	EnemyA() {};
	~EnemyA() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <returns></returns>
	static std::unique_ptr<EnemyA> Create(const XMFLOAT3& _pos);

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_pos">���W</param>
	void Initialize(const XMFLOAT3& _pos);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// �G�̈ړ������Z�b�g
	/// </summary>
	/// <param name="_pos">�v���C���[���W</param>
	void SetMove(const XMFLOAT3& _pos);

	/// <summary>
	/// �����߂��L�̓����蔻��
	/// </summary>
	void MapCollider();

private:

	//�^�C�}�[
	int timer;
	//�I�u�W�F�N�g
	std::unique_ptr<Object3d> object;
	//�����t���O
	bool isAlive;
	//���W
	XMFLOAT3 pos;
	//�ړ��x�N�g��
	XMFLOAT3 moveVec;
	//�傫��
	float scale;
	//�n�ʂɂ��Ă��邩
	bool onGround;
	//���ɏo���e�̗�
	const int bulletNum = 1;

public:

	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
	void SetIsAlive(const bool& _isAlive) { isAlive = _isAlive; }
};

