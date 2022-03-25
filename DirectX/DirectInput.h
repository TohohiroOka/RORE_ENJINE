#pragma once
#include "Singleton.h"
#include <DirectXMath.h>
#include<wrl.h>
#include<dinput.h>
#define DIRECTINPUT_VERSION 0x0800

class DirectInput : public Singleton< DirectInput >
{
friend Singleton< DirectInput >;

private://コンストラクタ&デストラクタ

	DirectInput() {};
	~DirectInput() {};

private:
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp">ウィンドウズインスタンス</param>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 特定のキーが押されているかのチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>押されているならtrue</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>トリガーで押されているならtrue</returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// マウスの左ボタンが押されているかのチェック
	/// </summary>
	/// <returns>押されているならtrue</returns>
	bool PushMouseLeft();

	/// <summary>
	/// マウスのカーソルボタンが押されているかのチェック
	/// </summary>
	/// <returns>押されているならtrue</returns>
	bool PushMouseCenter();

	/// <summary>
	/// マウスの右ボタンが押されているかのチェック
	/// </summary>
	/// <returns>押されているならtrue</returns>
	bool PushMouseRight();

	/// <summary>
	/// トリガーでのマウスの左ボタンが押されているかのチェック
	/// </summary>
	/// <returns>トリガーで押されているならtrue</returns>
	bool TriggerMouseLeft();

	/// <summary>
	/// トリガーでのマウスのカーソルボタンが押されているかのチェック
	/// </summary>
	/// <returns>トリガーで押されているならtrue</returns>
	bool TriggerMouseCenter();

	/// <summary>
	/// トリガーでのマウスの右ボタンが押されているかのチェック
	/// </summary>
	/// <returns>トリガーで押されているならtrue</returns>
	bool TriggerMouseRight();

private://変数
	//キー
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8>devkeyboard;
	BYTE key[256] = {};
	BYTE keyPre[256] = {};//前回キーの状態

	//マウス
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 mouseStatePre = {};
};