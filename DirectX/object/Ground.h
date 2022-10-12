#pragma once
#include "HeightMap.h"

class Ground
{
public:

	Ground() {};
	~Ground() {};

	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<Ground> Create(const std::string& heightmapFilename,
		const std::string& filename1 = "null", const std::string& filename2 = "null");

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	void Update();

	void Draw();

	void CDraw();

	void SetPos(DirectX::XMFLOAT3 pos);

private:

	std::unique_ptr<HeightMap> object = nullptr;
};