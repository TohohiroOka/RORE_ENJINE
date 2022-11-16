#pragma once
#include "BaseBullet.h"

class BossBulletBombHoming : public BaseBullet
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <param name="_color">�F</param>
	BossBulletBombHoming(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color);
	~BossBulletBombHoming() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <param name="_color">�F</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletBombHoming> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

private:

	int timer;
	int maxTimer;
};