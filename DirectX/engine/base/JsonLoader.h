#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <array>
#include <iostream>
#include <fstream>

class JsonLoader
{
private:

	struct Person
	{
		std::string name;
		float cameraDist;
		std::array<int,3> mapSize;
		std::vector<std::vector<std::vector<int>>> map;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(cameraDist), CEREAL_NVP(mapSize), CEREAL_NVP(map));
		}
	};

public:

	/// <summary>
	/// 出力
	/// </summary>
	/// <param name="_fileName">ファイルの名前</param>
	/// <param name="_cameraDist">カメラの距離</param>
	/// <param name="_mapSize">マップサイズ</param>
	/// <param name="_map">マップチップ</param>
	static void SerializeTest(const std::string& _fileName = "out.json", const float _cameraDist = 30.0f,
		const std::array<int, 3> _mapSize = { 10,10,10 }, std::vector<std::vector<std::vector<int>>> _map = { {{}} });

	/// <summary>
	/// 入力
	/// </summary>
	/// <param name="_fileName">ファイルの名前</param>
	/// <param name="_cameraDist">カメラの距離</param>
	/// <param name="_mapSize">マップサイズ</param>
	/// <param name="_map">マップチップ</param>
	static void DeserializeTest(std::string& _fileName, float _cameraDist,
		std::array<int, 3> _mapSize, std::vector<std::vector<std::vector<int>>> _map);
};