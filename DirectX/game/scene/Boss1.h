#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"
#include <array>

#include "BulletManager.h"
#include "EnemyManager.h"
#include "BossA.h"

#include "UiManager.h"

class Boss1 : public InterfaceScene
{
private:

	enum class SCENE {
		SET,//����
		PLAY,//�Q�[���v���C
		HOME,//�z�[�����
	};

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
	/// �X�V
	/// </summary>
	void CameraUpdate(Camera* camera) override;

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

private:

	//�V�[��
	SCENE scene;
	//�J�����̉�]
	float cameraAngle = -90;
	//�J�����̍���
	const float cameraY = 5.0f;

	//�v���C���[
	std::unique_ptr<Player> player;

	//�n�`
	std::unique_ptr<Ground> ground;

	//�e
	std::unique_ptr<BulletManager> bullet;
	
	//�G
	std::unique_ptr<EnemyManager> enemy;

	//�{�X
	std::unique_ptr<BossA> boss;

	//UI
	std::unique_ptr<UiManager> ui;
};