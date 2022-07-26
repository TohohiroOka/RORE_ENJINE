#pragma once
#include "HeightMap.h"

class Ground
{
public:

	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<Ground> Create(const std::string heightmapFilename, const std::string filename = "null");

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize();

	void Update();

	void Draw();

	void CDraw();

private:

	std::unique_ptr<HeightMap> object = nullptr;
};

