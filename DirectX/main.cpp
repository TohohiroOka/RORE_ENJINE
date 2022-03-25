#include "MainEngine.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウサイズ
	const int window_width = 1280;// 横幅
	const int window_height = 720;// 縦幅

	//ゲーム名
	const wchar_t* gameName = L"DirectXGame";

	//全体の初期化
	MainEngine* engine = MainEngine::GetInstance();
	engine->Initialize(gameName, window_width, window_height);

	while (true)
	{
		if (engine->Update() == true) { break; }
		engine->Draw();
		engine->FrameRateKeep();
	}

	engine->AllDelete();

	return 0;
}