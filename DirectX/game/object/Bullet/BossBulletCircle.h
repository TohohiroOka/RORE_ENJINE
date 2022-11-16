#pragma once
#include "BaseBullet.h"

class BossBulletCircle : public BaseBullet
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <param name="_speed">���x</param>
	BossBulletCircle(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const float _speed);
	~BossBulletCircle() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <param name="_speed">���x</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletCircle> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const float _speed);

	/// <summary>
	/// �F�ϊ�
	/// </summary>
	static void ColorChange();

private:

	//����̃_���}�N�̐F����
	static std::array<bool, 3> isCircleColor;
	//����̃_���}�N�̐F
	static XMFLOAT3 circleColor;

};