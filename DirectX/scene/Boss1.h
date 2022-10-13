#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"
#include <array>

#include "BulletManager.h"
#include "FixedTurret.h"
#include "EnemyManager.h"

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

	//�v���C���[
	std::unique_ptr<Player> player = nullptr;

	//�n�`
	static const int ground_num = 1;
	std::array<std::unique_ptr<Ground>, ground_num> ground;

	//�e
	std::unique_ptr<BulletManager> bullet;
	
	//�Œ�C��
	static const int fixed_turret_num = 4;
	std::array<std::unique_ptr<FixedTurret>, fixed_turret_num> fixedTurret;

	//�G
	std::unique_ptr<EnemyManager> enemy;
};