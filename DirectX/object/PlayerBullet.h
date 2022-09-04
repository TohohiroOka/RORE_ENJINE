#pragma once
#include "BaseBullet.h"

class PlayerBullet : public BaseBullet
{
private:

	struct INFORMATION
	{
		//�C���X�^���X
		std::unique_ptr<Object3d> instance = nullptr;
		//�����t���O
		bool isAlive = false;
		//�ړ��x�N�g��
		XMFLOAT3 vecMove = {};
	};

public:

	~PlayerBullet() override;

	/// <summary>
	/// �C���X�^���X����
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<PlayerBullet> Create();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// �e���ˎ��̐ݒ�
	/// </summary>
	/// <param name="_pos">�����l</param>
	/// <param name="_vecMove">�ړ��x�N�g��</param>
	void SetMove(const XMFLOAT3 _pos, const XMFLOAT3 _vecMove);

	/// <summary>
	/// ������
	/// </summary>
	void Reset();

private:

	const int bulletNum = 50;
	//�e
	std::vector<INFORMATION> object;
};

