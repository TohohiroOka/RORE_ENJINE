#pragma once
#include "InterfaceScene.h"
#include <array>

class Title : public InterfaceScene
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
	/// �X�V
	/// </summary>
	void CameraUpdate(Camera* camera) override {};

	/// <summary>
	/// �`��
	/// </summary>
	void DrawNotPostB() override;

	/// <summary>
	///	�`��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// �`��
	/// </summary>
	void DrawNotPostA() override;

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

	std::unique_ptr<Sprite> sp;
};