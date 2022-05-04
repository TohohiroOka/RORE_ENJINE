#pragma once
#include "InterfaceScene.h"
#include <array>

class BrowsingCircle : public InterfaceScene
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

	//�~���f��
	std::unique_ptr<FbxModel> SpherePBRModel = nullptr;

	//�~
	std::array<std::unique_ptr<Fbx>, 25> circle;

};

