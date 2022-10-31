#pragma once
#include "HeightMap.h"
#include "Object3d.h"

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

	void CubeDraw();

	void SetPos(DirectX::XMFLOAT3 pos);

private:

	//マップを覆うキューブ
	std::array<std::unique_ptr<Model>, 2> cubeModel;
	std::array<std::unique_ptr<Object3d> ,2> cubeObject;
	//マップ
	static const float scale;
	std::unique_ptr<HeightMap> object = nullptr;

public:

	static float GetScale() { return scale; }
};