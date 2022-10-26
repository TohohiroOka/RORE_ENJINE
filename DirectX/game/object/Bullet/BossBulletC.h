#pragma once
#include "BaseBullet.h"

class BossBulletC :
    public BaseBullet
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_targetPos">�^�[�Q�b�g���W</param>
	/// <param name="_color">�F</param>
	BossBulletC(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, const XMFLOAT3& _color);
	~BossBulletC() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_targetPos">�^�[�Q�b�g���W</param>
	/// <param name="_color">�F</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletC> Create(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, const XMFLOAT3& _color);
};

