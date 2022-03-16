#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <commdlg.h>
#include <basetsd.h>
#include <objbase.h>

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#include <XInput.h>
#pragma comment(lib,"xinput.lib")
#else
#include <XInput.h>
#pragma comment(lib,"xinput9_1_0.lib")
#endif

#include <DirectXMath.h>

#include "Singleton.h"

class XInputManager : public Singleton< XInputManager >
{
	friend Singleton< XInputManager >;

private://�R���X�g���N�^&�f�X�g���N�^

	XInputManager() {};
	~XInputManager();

private://�����o�\����

	struct CONTROLLER_STATE
	{
		XINPUT_STATE lastState;
		XINPUT_STATE state;
		DWORD dwResult;
		bool bLockVibration;
		XINPUT_VIBRATION vibration;
	};

public://�����oEnum

	//�Q�[���p�b�h�L�[�R���t�B�O
	enum PudButton {
		PAD_A = 0,//A
		PAD_B,//B
		PAD_Y,//Y
		PAD_X,//X
		PAD_LB,//LB
		PAD_RB,//RB
		PAD_LT,//LT
		PAD_RT,//RT
		PAD_START,//START
		PAD_BUCK,//BUCK
		PAD_UP,//�\����
		PAD_DOWN,//�\����
		PAD_LEFT,//�\����
		PAD_RIGHT,//�\���E
		PAD_LEFT_STICK_PUSH,//���X�e�b�N��������
		PAD_RIGHT_STICK_PUSH,//�E�X�e�b�N��������
	};


public://�����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �w��{�^����������Ă��邩
	/// </summary>
	/// <param name="button">�{�^��</param>
	/// <returns>������Ă��邩�ۂ�</returns>
	bool PushButton(PudButton button);

	/// <summary>
	/// �w��{�^���̃g���K�[���`�F�b�N
	/// </summary>
	/// <param name="button">�{�^��</param>
	/// <returns>������Ă��邩�ۂ�</returns>
	bool TriggerButton(PudButton button);

	/// <summary>
	/// ���X�e�B�b�N��X���`�F�b�N
	/// </summary>
	/// <param name="UpDown">Up -> true / Down -> false</param>
	/// <returns>true/false</returns>
	bool LeftStickX(bool UpDown);

	/// <summary>
	/// ���X�e�B�b�N��Y���`�F�b�N
	/// </summary>
	/// <param name="LeftRight">Left -> true / Right -> false</param>
	/// <returns>true/false</returns>
	bool LeftStickY(bool LeftRight);

	/// <summary>
	/// �E�X�e�B�b�N��X���`�F�b�N
	/// </summary>
	/// <param name="UpDown">Up -> true / Down -> false</param>
	/// <returns>true/false</returns>
	bool RightStickX(bool UpDown);

	/// <summary>
	/// �E�X�e�B�b�N��Y���`�F�b�N
	/// </summary>
	/// <param name="LeftRight">Left -> true / Right -> false</param>
	/// <returns>true/false</returns>
	bool RightStickY(bool LeftRight);

	/// <summary>
	/// ���X�e�B�b�N��X���g���K�[�Ń`�F�b�N
	/// </summary>
	/// <param name="UpDown">Up -> true / Down -> false</param>
	/// <returns>true/false</returns>
	bool TriggerLeftStickX(bool UpDown);

	/// <summary>
	/// ���X�e�B�b�N��Y���g���K�[�Ń`�F�b�N
	/// </summary>
	/// <param name="LeftRight">Left -> true / Right -> false</param>
	/// <returns>true/false</returns>
	bool TriggerLeftStickY(bool LeftRight);

	/// <summary>
	/// �E�X�e�B�b�N��X���g���K�[�Ń`�F�b�N
	/// </summary>
	/// <param name="UpDown">Up -> true / Down -> false</param>
	/// <returns>true/false</returns>
	bool TriggerRightStickX(bool UpDown);

	/// <summary>
	/// �E�X�e�B�b�N��Y���g���K�[�Ń`�F�b�N
	/// </summary>
	/// <param name="LeftRight">Left -> true / Right -> false</param>
	/// <returns>true/false</returns>
	bool TriggerRightStickY(bool LeftRight);

	/// <summary>
	/// �Q�[���p�b�h�̍��X�e�B�b�N�̌X�����擾
	/// </summary>
	/// <returns>�Q�[���p�b�h�̍��X�e�B�b�N�̌X��(-1�`1)</returns>
	DirectX::XMFLOAT2 GetPadLStickIncline();

	/// <summary>
	/// �Q�[���p�b�h�̉E�X�e�B�b�N�̌X�����擾
	/// </summary>
	/// <returns>�Q�[���p�b�h�̉E�X�e�B�b�N�̌X��(-1�`1)</returns>
	DirectX::XMFLOAT2 GetPadRStickIncline();

	/// <summary>
	/// �Q�[���p�b�h�̍��X�e�B�b�N�̊p�x���擾
	/// </summary>
	/// <returns>�Q�[���p�b�h�̍��X�e�B�b�N�̊p�x</returns>
	float GetPadLStickAngle();

	/// <summary>
	/// �Q�[���p�b�h�̉E�X�e�B�b�N�̊p�x���擾
	/// </summary>
	/// <returns>�Q�[���p�b�h�̉E�X�e�B�b�N�̊p�x</returns>
	float GetPadRStickAngle();

	/// <summary>
	/// �U���J�n
	/// </summary>
	void StartVibration();

	/// <summary>
	/// �U���J�n
	/// </summary>
	void EndVibration();

private://�����o�ϐ�

	CONTROLLER_STATE g_Controllers;
};