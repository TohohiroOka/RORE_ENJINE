#pragma once
#include "BaseBullet.h"
#include "DrawLine3D.h"

class EnemyABullet : public BaseBullet
{
public:

	EnemyABullet() {};
	~EnemyABullet() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <returns></returns>
	static std::unique_ptr<EnemyABullet> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	void Initialize(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// �`��
	/// </summary>
	void DLDraw();

private:

	std::unique_ptr<Object3d> object;
	//�����t���O
	bool isAlive;
	//���W
	XMFLOAT3 pos;
	//�ړ��x�N�g��
	XMFLOAT3 moveVec;
	//�傫��
	float scale;
	//�����蔻��p
	std::unique_ptr<DrawLine3D> hitObject;

public:

	void SetIsAlive(const bool& _isAlive) { isAlive = _isAlive; }
	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
};
