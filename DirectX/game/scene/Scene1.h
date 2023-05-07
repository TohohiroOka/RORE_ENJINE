#pragma once
#include "InterfaceScene.h"
#include "Map.h"
#include "Vector3.h"

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

	/// <summary>
	/// �X�N���[�����W���烏�[���h���W�ւ̕ϊ�
	/// </summary>
	/// <param name="pout">���[���h���W</param>
	/// <param name="_screenPos">�X�N���[�����W</param>
	/// <param name="fZ">�ˉe��Ԃł�Z�l�i0�`1</param>
	/// <returns>���[���h���W</returns>
	Vector3* CalcScreenToWorld(Vector3* _pout, XMFLOAT2 _screenPos, float fz);

private:

	Camera* camera;

	//�J�������W
	XMFLOAT3 cameraPos;
	//�J�������W
	XMFLOAT3 cameraTarget;

	//��
	std::unique_ptr<Map> map;
	//����`�悷�邩�̃t���O
	bool isDrawLine;
	//�I�u�W�F�N�g�ݒu�t���O
	bool isAdd;
	bool frame;

	//imgui
	XMFLOAT2 imguiPos;
	const XMFLOAT2 imguiMax = { 300,300 };
	bool kaburi;
};