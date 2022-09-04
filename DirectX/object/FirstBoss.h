#pragma once
#include "BaseEnemy.h"

class FirstBoss : public BaseEnemy
{
public:

	~FirstBoss() override;

	/// <summary>
	/// �C���X�^���X����
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<FirstBoss> Create();

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
	/// ���W�擾
	/// </summary>
	XMFLOAT3 GetPosition() override;

private://�萔

	//�{�X�̑̂̐�
	static const int bossBodyNum = 8;
	//�R�A����̋���
	static const XMFLOAT3 bodyDistance[bossBodyNum];

private:

	//�{�X�̃R�A
	std::unique_ptr<Object3d> core = nullptr;
	//�{�X�̑�
	std::array<std::unique_ptr<Object3d>, bossBodyNum> body;
};