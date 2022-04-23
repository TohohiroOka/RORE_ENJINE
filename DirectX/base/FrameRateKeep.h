#pragma once
#include <chrono>
#include <d3dx12.h>

class WindowApp;

class FrameRateKeep
{
private:

public:

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<FrameRateKeep> Create();

	/// <summary>
	/// Fps値の固定
	/// </summary>
	void FixedFps(WindowApp* winApp);

public:

	//スタート時間
	float deltaTime = 0.0f;
	float frameRate = 0.0f;
	float commandWaitTime = 0.0f;
	std::chrono::steady_clock::time_point lastUpdate;


};

