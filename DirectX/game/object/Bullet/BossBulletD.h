#pragma once
#include "BaseBullet.h"

class BossBulletD :
    public BaseBullet
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	BossBulletD(const XMFLOAT3& _pos);
	~BossBulletD() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_color">�F</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletD> Create(const XMFLOAT3& _pos, const XMFLOAT3& _color);

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	void Initialize() override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

private:

	//�^�C�}�[
	int timer;
	//����l�i�����_���j
	int divideNum;
	//��]�p
	float angle;
	//���x
	float velocity;
};
