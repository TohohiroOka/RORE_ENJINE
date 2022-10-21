#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"
#include <array>

#include "BulletManager.h"
#include "EnemyManager.h"
#include "BossA.h"

class Boss1 : public InterfaceScene
{
public:

	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

	/// <summary>
	///	�X�V
	/// </summary>
	void Update() override;

	/// <summary>
	///	�`��
	/// </summary>
	void Draw() override;

	/// <summary>
	///	���
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// imgui�̕\��
	/// </summary>
	void ImguiDraw() override;

	/// <summary>
	/// �J�����̍X�V
	/// </summary>
	void CameraUpdate();

private:

	//�J�����̉�]
	float cameraAngle = -90;
	//�J�����̍���
	const float cameraY = 30.0f;
	//�^�[�Q�b�g
	bool isTarget;
	//�^�[�Q�b�g���̃C�[�W���O
	bool isTageEase;
	//�C�[�W���O�^�C�}�[
	float easeTimer;
	//�C�[�W���O�����p�x
	float startAngle;
	//�C�[�W���O�����l�_
	XMFLOAT3 startTarget;
	//�C�[�W���O�����l�_
	XMFLOAT3 startPos;

	//�v���C���[
	std::unique_ptr<Player> player = nullptr;

	//�n�`
	static const int ground_num = 1;
	std::array<std::unique_ptr<Ground>, ground_num> ground;

	//�e
	std::unique_ptr<BulletManager> bullet;
	
	//�G
	std::unique_ptr<EnemyManager> enemy;

	//�{�X
	std::unique_ptr<BossA> boss;
};