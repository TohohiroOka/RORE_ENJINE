#include "Input.h"
#include "WindowApp.h"
#include <cassert>

//ライブラリのリンク
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

// デバイス発見時に実行される
BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	return DIENUM_CONTINUE;
}

void Input::Initialize()
{
	HRESULT result;

	//DirectInputのインスタンス生成
	result = DirectInput8Create(WindowApp::GetWinInstance(), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&dinput, nullptr);
	
	//キーボードデバイス生成
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
	// マウスデバイスの生成	
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);

	//入力データ形式のセット(キー)
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
	//排他制御レベルのセット(キー)
	result = devkeyboard->SetCooperativeLevel(WindowApp::GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	// 入力データ形式のセット(マウス)
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // 標準形式
	// 排他制御レベルのセット(マウス)
	result = devMouse->SetCooperativeLevel(WindowApp::GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

bool Input::PushKey(BYTE keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	//指定キーを押していなければtrueを返す
	if (key[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	//指定キーを押していなければtrueを返す
	if (key[keyNumber]&& !keyPre[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

bool Input::PushMouseLeft()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[0]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::PushMouseCenter()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[1]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::PushMouseRight()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[2]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::TriggerMouseLeft()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[0] && mouseState.rgbButtons[0]) {
		return true;
	}

	// トリガーでない
	return false;
}

bool Input::TriggerMouseCenter()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[1] && mouseState.rgbButtons[1]) {
		return true;
	}

	// トリガーでない
	return false;
}

bool Input::TriggerMouseRight()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[2] && mouseState.rgbButtons[2]) {
		return true;
	}

	// トリガーでない
	return false;
}

void Input::Update()
{
	HRESULT result;

	//-----------キー--------------//
	//キーボード情報の取得開始
	result = devkeyboard->Acquire();

	//前回キー入力を保存
	memcpy(keyPre, key, sizeof(key));

	//全キーの入力状態を取得する
	result = devkeyboard->GetDeviceState(sizeof(key), key);

	//-----------マウス--------------//
	// マウス動作開始
	result = devMouse->Acquire();

	// 前回の入力を保存
	mouseStatePre = mouseState;

	// マウスの入力
	result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);
}
