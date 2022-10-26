#pragma once
#include "BaseBullet.h"

class BossBulletE :
	public BaseBullet
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_color">�F</param>
	BossBulletE(const XMFLOAT3& _pos, const XMFLOAT3& _color);
	~BossBulletE() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_color">�F</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletE> Create(const XMFLOAT3& _pos, const XMFLOAT3& _color);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

private:

	//�^�C�}�[
	int timer;
	//�ړ��t���O
	bool isMove;
};