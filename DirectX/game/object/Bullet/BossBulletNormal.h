#pragma once
#include "BaseBullet.h"

class BossBulletNormal : public BaseBullet
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <param name="_speed">���x</param>
	/// <param name="_color">�F</param>
	BossBulletNormal(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, float _speed, const XMFLOAT3& _color);
	~BossBulletNormal() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <param name="_speed">���x</param>
	/// <param name="_color">�F</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletNormal> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec,
		float _speed, const XMFLOAT3& _color);
};