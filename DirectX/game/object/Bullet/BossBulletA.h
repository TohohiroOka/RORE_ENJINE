#pragma once
#include "BaseBullet.h"

class BossBulletA : public BaseBullet
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	BossBulletA(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);
	~BossBulletA() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletA> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	void Initialize() override;

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