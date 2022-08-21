#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"

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
	/// �R���s���[�g�V�F�[�_�[����̌v�Z���ʎ擾
	/// </summary>
	void GetConstbufferNum() override;

private:

	//�J�����̉�]
	float cameraAngle = 100;
	//�J�����̍���
	float cameraY = 50.0f;


	std::unique_ptr<Model> playerModel = nullptr;
	std::unique_ptr<Model> groundModel = nullptr;

	std::unique_ptr<Player> player = nullptr;
	std::unique_ptr<Ground> ground = nullptr;

	std::unique_ptr<TouchableObject> tmap = nullptr;

	bool isDraw = true;
};