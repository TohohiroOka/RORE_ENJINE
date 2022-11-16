#pragma once
#include "BaseBullet.h"

class BossBulletSnake :
    public BaseBullet
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_color">�F</param>
	BossBulletSnake(const XMFLOAT3& _pos, const XMFLOAT3& _color);
	~BossBulletSnake() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_color">�F</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletSnake> Create(const XMFLOAT3& _pos, const XMFLOAT3& _color);

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

