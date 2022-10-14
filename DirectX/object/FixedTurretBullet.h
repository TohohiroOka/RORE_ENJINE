#pragma once
#include "BaseBullet.h"

class FixedTurretBullet : public BaseBullet
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	FixedTurretBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);
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
	void Initialize() override;
};