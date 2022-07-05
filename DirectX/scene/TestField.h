#pragma once
#include "InterfaceScene.h"

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

	//�J�������W
	XMFLOAT3 cameraPos = {};
	//�J�����̉�]
	float cameraAngle = 100;
	//�J�����̍���
	float cameraY = 50.0f;

	std::unique_ptr<HeightMap> heightmap = nullptr;
	
};