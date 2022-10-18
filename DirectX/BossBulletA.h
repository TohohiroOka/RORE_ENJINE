#pragma once
#include "BaseBullet.h"

class BossBulletA :
    public BaseBullet
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_angleXZ">xz���ʂ̊p�x</param>
	/// <param name="_angleXY">xy���ʂ̊p�x</param>
	/// <param name="_color">�F</param>
	BossBulletA(const XMFLOAT3& _pos, float _angleXZ, float _angleXY, const XMFLOAT4& _color);
	~BossBulletA() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_angleXZ">xz���ʂ̊p�x</param>
	/// <param name="_angleXY">xy���ʂ̊p�x</param>
	/// <param name="_color">�F</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletA> Create(const XMFLOAT3& _pos, float _angleXZ, float _angleXY, const XMFLOAT4& _color);

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	void Initialize() override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

private:

	//xz���ʂ̊p�x
	float angleXZ;
	//xy���ʂ̊p�x
	float angleXY;
	//���S����̋���
	float distance;

};

