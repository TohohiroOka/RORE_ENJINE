#pragma once
#include "BaseBullet.h"

class FixedTurretBullet : public BaseBullet
{
public:

	FixedTurretBullet() {};
	~FixedTurretBullet() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <returns></returns>
	static std::unique_ptr<FixedTurretBullet> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);

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

public:

	void SetIsAlive(bool _isAlive) { isAlive = _isAlive; }
	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
};