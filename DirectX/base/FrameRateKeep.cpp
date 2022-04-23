#include "FrameRateKeep.h"
#include "WindowApp.h"

std::unique_ptr<FrameRateKeep> FrameRateKeep::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	FrameRateKeep* instance = new FrameRateKeep();
	if (instance == nullptr) {
		return nullptr;
	}

	return std::unique_ptr<FrameRateKeep>(instance);

}

void FrameRateKeep::FixedFps(WindowApp* winApp)
{
	// 経過時間計測
	auto now = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
	frameRate = 1.0f / deltaTime;
	lastUpdate = now;

	// FPS,CPU使用率表示
	{
		static int count = 0;
		const float FPS_BASIS = 60.0f;
		// 一秒に一度更新
		if (++count > FPS_BASIS) {
			count = 0;
			float cputime = deltaTime - commandWaitTime;
			char str[50];
			sprintf_s(str, 50, "fps=%03.0f cpu usage=%06.2f%%", frameRate, cputime * FPS_BASIS * 100.0f);
			SetWindowTextA(winApp->GetHwnd(), str);
		}
	}
}