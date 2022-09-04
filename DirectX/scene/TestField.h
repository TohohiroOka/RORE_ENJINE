#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"
#include <array>

#include "EnemyManager.h"

class TestField : public InterfaceScene
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
	float cameraAngle = 100;
	//�J�����̍���
	const float cameraY = 30.0f;

	//�v���C���[
	std::unique_ptr<Player> player = nullptr;

	//�n�`
	static const int ground_num = 1;
	std::array<std::unique_ptr<Ground>, ground_num> ground;

	//�G
	std::unique_ptr<EnemyManager> enemy = nullptr;
};