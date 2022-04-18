#pragma once
#include "DirectXCommon.h"
#include "GameScene.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "PostEffect.h"
#include "Camera.h"

#include <sstream>
#include <iomanip>
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")

class MainEngine
{
public://メインに書く

	MainEngine() = default;
	~MainEngine();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <returns>ゲームを続けるか</returns>
	bool Update();

	/// <summary>
	/// デバッグ用数字
	/// </summary>
	/// <param name="x">値1</param>
	/// <param name="y">値2</param>
	/// <param name="z">値3</param>
	void debugNum(float x, float y, float z);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// フレームレート固定
	/// </summary>
	void frameRateKeep();

private:

	//DirectXCommonのインスタンス
	DirectXCommon* dXCommon = nullptr;
	//Inputのインスタンス
	DirectInput* input = nullptr;
	//XInputManager
	XInputManager* Xinput = nullptr;
	//Cameraのインスタンス
	Camera* camera = nullptr;
	//GameSceneのインスタンス
	GameScene* scene = nullptr;
	//ポストエフェクトのインスタンス
	std::unique_ptr<PostEffect> postEffect = nullptr;
	//数字表示デバッグ用
	wchar_t str[256] = {};
	//フレームレート固定用
	const float MIN_FREAM_TIME = 1.0f / 60;
	//フレームのカウント
	float frameTime = 0;
	//フレーム始め
	LARGE_INTEGER timeStart;
	//フレーム最後
	LARGE_INTEGER timeEnd;
	//現在フレーム
	LARGE_INTEGER timeFreq;
	//現在FPS
	float fps = 0;
};