#pragma once
#include "WindowApp.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "Input.h"
#include "XInputManager.h"
#include "Singleton.h"
#include "PostEffect.h"

#include <mmsystem.h>
#include<sstream>
#include <iomanip>

#pragma comment(lib,"winmm.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

class MainEngine : public Singleton< MainEngine >
{
	friend Singleton< MainEngine >;

private://コンストラクタ&デストラクタ

	MainEngine() {};
	~MainEngine();

public://メンバ関数

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="gameName">タイトルバー名</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	void Initialize(const wchar_t* gameName, int window_width, int window_height);

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
	void DebugNum(float x, float y, float z);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// フレームレート固定
	/// </summary>
	void FrameRateKeep();

	/// <summary>
	/// エスケープが入力されたら終了する処理
	/// </summary>
	/// <param name="winApp">ウィンドウズインスタンス</param>
	/// <returns>ゲームを続けるか</returns>
	bool GameFin();

	/// <summary>
	/// Cameraのgetter
	/// </summary>
	/// <returns>Cameraのインスタンス</returns>
	Camera* GetCamera() { return camera; }

private:

	//WindowAppのインスタンス
	WindowApp* winApp;
	//DirectXCommonのインスタンス
	DirectXCommon* dXCommon;
	//Inputのインスタンス
	Input* input;
	//XInputのインスタンス
	XInputManager* xinput;
	//GameSceneのインスタンス
	GameScene* scene;
	//ポストエフェクト
	PostEffect* postEffect = nullptr;
	//Cameraのインスタンス
	Camera* camera = nullptr;
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

private:

	XMFLOAT2 scale = { 1,1 };
	bool F[2] = { false,true };
};