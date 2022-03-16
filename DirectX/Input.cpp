#include "Input.h"
#include "WindowApp.h"
#include <cassert>

//���C�u�����̃����N
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

// �f�o�C�X�������Ɏ��s�����
BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	return DIENUM_CONTINUE;
}

void Input::Initialize()
{
	HRESULT result;

	//DirectInput�̃C���X�^���X����
	result = DirectInput8Create(WindowApp::GetWinInstance(), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&dinput, nullptr);
	
	//�L�[�{�[�h�f�o�C�X����
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
	// �}�E�X�f�o�C�X�̐���	
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);

	//���̓f�[�^�`���̃Z�b�g(�L�[)
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
	//�r�����䃌�x���̃Z�b�g(�L�[)
	result = devkeyboard->SetCooperativeLevel(WindowApp::GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	// ���̓f�[�^�`���̃Z�b�g(�}�E�X)
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // �W���`��
	// �r�����䃌�x���̃Z�b�g(�}�E�X)
	result = devMouse->SetCooperativeLevel(WindowApp::GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

bool Input::PushKey(BYTE keyNumber)
{
	// �ُ�Ȉ��������o
	assert(0 <= keyNumber && keyNumber <= 256);

	//�w��L�[�������Ă��Ȃ����true��Ԃ�
	if (key[keyNumber]) {
		return true;
	}
	//�����łȂ����false��Ԃ�
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	// �ُ�Ȉ��������o
	assert(0 <= keyNumber && keyNumber <= 256);

	//�w��L�[�������Ă��Ȃ����true��Ԃ�
	if (key[keyNumber]&& !keyPre[keyNumber]) {
		return true;
	}
	//�����łȂ����false��Ԃ�
	return false;
}

bool Input::PushMouseLeft()
{
	// 0�łȂ���Ή����Ă���
	if (mouseState.rgbButtons[0]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;
}

bool Input::PushMouseCenter()
{
	// 0�łȂ���Ή����Ă���
	if (mouseState.rgbButtons[1]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;
}

bool Input::PushMouseRight()
{
	// 0�łȂ���Ή����Ă���
	if (mouseState.rgbButtons[2]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;
}

bool Input::TriggerMouseLeft()
{
	// �O��0�ŁA����0�łȂ���΃g���K�[
	if (!mouseStatePre.rgbButtons[0] && mouseState.rgbButtons[0]) {
		return true;
	}

	// �g���K�[�łȂ�
	return false;
}

bool Input::TriggerMouseCenter()
{
	// �O��0�ŁA����0�łȂ���΃g���K�[
	if (!mouseStatePre.rgbButtons[1] && mouseState.rgbButtons[1]) {
		return true;
	}

	// �g���K�[�łȂ�
	return false;
}

bool Input::TriggerMouseRight()
{
	// �O��0�ŁA����0�łȂ���΃g���K�[
	if (!mouseStatePre.rgbButtons[2] && mouseState.rgbButtons[2]) {
		return true;
	}

	// �g���K�[�łȂ�
	return false;
}

void Input::Update()
{
	HRESULT result;

	//-----------�L�[--------------//
	//�L�[�{�[�h���̎擾�J�n
	result = devkeyboard->Acquire();

	//�O��L�[���͂�ۑ�
	memcpy(keyPre, key, sizeof(key));

	//�S�L�[�̓��͏�Ԃ��擾����
	result = devkeyboard->GetDeviceState(sizeof(key), key);

	//-----------�}�E�X--------------//
	// �}�E�X����J�n
	result = devMouse->Acquire();

	// �O��̓��͂�ۑ�
	mouseStatePre = mouseState;

	// �}�E�X�̓���
	result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);
}
