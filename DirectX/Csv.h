#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>

class Csv
{
protected: // エイリアス
// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;

public:

	/// <summary>
	/// マップの読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <param name="map"></param>
	static void LoadFile(std::string fileName, std::vector<std::vector<int>> map);

	/// <summary>
	/// オブジェクト座標の配列番号の値の取得
	/// </summary>
	/// <param name="map">二次元配列</param>
	/// <param name="chipSize">一つ分の大きさ</param>
	/// <param name="position">オブジェクト座標</param>
	/// <returns></returns>
	static int GetChipNum(std::vector<std::vector<int>> map, float chipSize, XMFLOAT2 position);
};