#pragma once
#include "Singleton.h"
#include <DirectXMath.h>
#include<wrl.h>
#include<dinput.h>
#define DIRECTINPUT_VERSION 0x0800

class DirectInput : public Singleton< DirectInput >
{
friend Singleton< DirectInput >;

private://�R���X�g���N�^&�f�X�g���N�^

	DirectInput() {};
	~DirectInput() {};

private:
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://�����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="winApp">�E�B���h�E�Y�C���X�^���X</param>
	void Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// ����̃L�[��������Ă��邩�̃`�F�b�N
	/// </summary>
	/// <param name="keyNumber">�L�[�ԍ�(DIK_0��)</param>
	/// <returns>������Ă���Ȃ�true</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// �L�[�̃g���K�[���`�F�b�N
	/// </summary>
	/// <param name="keyNumber">�L�[�ԍ�(DIK_0��)</param>
	/// <returns>�g���K�[�ŉ�����Ă���Ȃ�true</returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// �}�E�X�̍��{�^����������Ă��邩�̃`�F�b�N
	/// </summary>
	/// <returns>������Ă���Ȃ�true</returns>
	bool PushMouseLeft();

	/// <summary>
	/// �}�E�X�̃J�[�\���{�^����������Ă��邩�̃`�F�b�N
	/// </summary>
	/// <returns>������Ă���Ȃ�true</returns>
	bool PushMouseCenter();

	/// <summary>
	/// �}�E�X�̉E�{�^����������Ă��邩�̃`�F�b�N
	/// </summary>
	/// <returns>������Ă���Ȃ�true</returns>
	bool PushMouseRight();

	/// <summary>
	/// �g���K�[�ł̃}�E�X�̍��{�^����������Ă��邩�̃`�F�b�N
	/// </summary>
	/// <returns>�g���K�[�ŉ�����Ă���Ȃ�true</returns>
	bool TriggerMouseLeft();

	/// <summary>
	/// �g���K�[�ł̃}�E�X�̃J�[�\���{�^����������Ă��邩�̃`�F�b�N
	/// </summary>
	/// <returns>�g���K�[�ŉ�����Ă���Ȃ�true</returns>
	bool TriggerMouseCenter();

	/// <summary>
	/// �g���K�[�ł̃}�E�X�̉E�{�^����������Ă��邩�̃`�F�b�N
	/// </summary>
	/// <returns>�g���K�[�ŉ�����Ă���Ȃ�true</returns>
	bool TriggerMouseRight();

private://�ϐ�
	//�L�[
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8>devkeyboard;
	BYTE key[256] = {};
	BYTE keyPre[256] = {};//�O��L�[�̏��

	//�}�E�X
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 mouseStatePre = {};
};