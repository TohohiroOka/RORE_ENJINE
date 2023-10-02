#include "WindowApp.h"
#include "MainEngine.h"
#include "SafeDelete.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウサイズ
	const int window_width = 1500;// 横幅
	const int window_height = 830;// 縦幅

	//ゲーム名
	const wchar_t* gameName = L"DirectX";

	//ウィンドウ初期化
	WindowApp* winApp = new WindowApp();
	winApp->Initialize(window_width, window_height, gameName);

	//全体の初期化
	MainEngine* engine = new MainEngine();
	engine->Initialize();

	SYSTEMTIME StartTime;
	SYSTEMTIME NowTime;
	__int64 m_ProgressTime;

	while (true)
	{
		if (engine->Update()|| winApp->Update()) { break; }
		engine->Draw();

		//フレームレート管理
		engine->FrameControl();

		while (1) {
			// 現在時刻
			GetLocalTime(&NowTime);
			// 変換
			FILETIME ftime1;
			FILETIME ftime2;
			SystemTimeToFileTime(&StartTime, &ftime1);
			SystemTimeToFileTime(&NowTime, &ftime2);
			// int64にキャスト
			__int64* nTime1 = (__int64*)&ftime1;
			__int64* nTime2 = (__int64*)&ftime2;
			// 経過秒
			m_ProgressTime = (*nTime2 - *nTime1);
			if (m_ProgressTime >= __int64(100000)) {
				break;
			}
		}

	}

	//登録解除
	safe_delete(engine);
	winApp->Release();

	return 0;
}