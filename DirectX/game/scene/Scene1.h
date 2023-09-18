#pragma once
#include "Scene/InterfaceScene.h"
#include "Math/Vector3.h"
#include "Object/3d/HeightMap.h"
#include "Player.h"

class Scene1 : public InterfaceScene
{
private:// �G�C���A�X
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMINT3 = DirectX::XMINT3;

public:

	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

	/// <summary>
	///	�X�V
	/// </summary>
	void Update()override;

	/// <summary>
	/// �X�V
	/// </summary>
	void CameraUpdate(const int _cameraNum, Camera* camera) override;

	/// <summary>
	///	�`��
	/// </summary>
	/// <param name="_cameraNum">�J�����ԍ�</param>
	void Draw(const int _cameraNum) override;
	
	/// <summary>
	/// �|�X�g�G�t�F�N�g�������Ȃ��`��
	/// </summary>
	void NonPostEffectDraw(const int _cameraNum) override;

	/// <summary>
	///	���
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// imgui�̕\��
	/// </summary>
	void ImguiDraw() override;

	/// <summary>
	/// 
	/// </summary>
	void FrameReset() override;

private:

	Camera* camera;

	//�J�������W
	XMFLOAT3 cameraPos;
	//�J�������W
	XMFLOAT3 cameraTarget;

	std::unique_ptr<Sprite> sprite;

	//�n�`���
	std::unique_ptr<TerrainModel> m_model;
	//�`��ݒ�
	std::unique_ptr<HeightMap> object;

	std::unique_ptr<Player> player;

};