#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "BulletManager.h"

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
	/// �X�V
	/// </summary>
	void CameraUpdate(Camera* camera) override;

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

private:

	//�J�����̉�]
	XMFLOAT2 cameraAngle;

	std::unique_ptr<Player> player = nullptr;
	std::unique_ptr<HeightMap> map=nullptr;
	std::unique_ptr<BulletManager> bullet = nullptr;

	int houkou=1;
};