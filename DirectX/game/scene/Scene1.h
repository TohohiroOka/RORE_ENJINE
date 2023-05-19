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

	//�}�b�v��
	static char fileName[36];

	//��
	std::unique_ptr<Map> map;
	//����`�悷�邩�̃t���O
	bool isDrawLine;

	//�ݒu�I�u�W�F�N�g�Ǘ�/0�v���C���[/1�S�[��/2�m�[�}��
	std::array<bool, 9> isSetObject;

	//imgui
	XMFLOAT2 imguiPos;
	const XMFLOAT2 imguiMax = { 300,300 };
	bool kaburi;

	//�}�b�v�`�F���W
	bool mapChange;
	XMINT3 mapChangeSize;

	//�^�C�}�[
	int exportTimer;
	int improtTimer;
	bool isImprot;

	//imgui�J���[
	XMFLOAT4 imguiColor;

	//�O���̖ʂƂ̔���
	bool isOutsideCollision;

	//�ڒn�ʂ̒ǉ�����
	bool mapChangeDirection;
};