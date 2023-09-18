#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <cereal/archives/binary.hpp>
#include <array>
#include <iostream>
#include <fstream>

class JsonLoader
{
private:

	struct Json
	{
		std::string name;
		std::array<float, 3> cameraDist;
		int installationSurface;
		std::array<int, 3> mapSize;
		std::vector<std::vector<std::vector<int>>> map;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(cameraDist), CEREAL_NVP(installationSurface), CEREAL_NVP(mapSize), CEREAL_NVP(map));
		}
	};

	struct Binary
	{
		std::array<int, 3> mapSize;
		std::vector<std::vector<std::vector<int>>> map;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(CEREAL_NVP(mapSize), CEREAL_NVP(map));
		}
	};

public:

	struct Shader {
		std::string name;
		std::vector<std::string> type;
	};

	struct Pipeline {
		std::string name;
		std::vector<std::string> shaderType;
		std::vector<std::string> inputLayoutType;
		int rtvNum;
		std::string blendMode;
		std::string drawMode;
		std::string drawType;
		int textureNum;
		int rootparams;
	};

public:

	/// <summary>
	/// jsonの読み込み
	/// </summary>
	/// <param name="_fileName">ファイル名</param>
	/// <param name="_shaderName">シェーダー名</param>
	static void LoadShaderName(const std::string& _fileName, std::vector<Shader>* _shaderName);

	/// <summary>
	/// jsonの読み込み
	/// </summary>
	/// <param name="_fileName">ファイル名</param>
	/// <param name="_pipeline">パイプライン情報</param>
	static void LoadPipeline(const std::string& _fileName, std::vector<Pipeline>* _pipeline);

	/// <summary>
	/// 出力
	/// </summary>
	/// <param name="_fileName">ファイルの名前</param>
	/// <param name="_cameraDist">カメラの距離</param>
	/// <param name="_mapSize">マップサイズ</param>
	/// <param name="_map">マップチップ</param>
	static void SerializeJson(const std::string& _fileName = "out.json", const std::array<float, 3> _cameraDist = { 30.0f ,30.0f ,30.0f },
		const std::array<int, 3> _mapSize = { 10,10,10 }, std::vector<std::vector<std::vector<int>>> _map = { {{}} });

	/// <summary>
	/// 入力
	/// </summary>
	/// <param name="_fileName">ファイルの名前</param>
	/// <param name="_cameraDist">カメラの距離</param>
	/// <param name="_map">マップチップ</param>
	static bool DeserializeJson(const std::string _fileName, std::array<float, 3>* _cameraDist,
		std::vector<std::vector<std::vector<int>>>* _map);

	/// <summary>
	/// 出力
	/// </summary>
	/// <param name="_fileName">ファイルの名前</param>
	/// <param name="_mapSize">マップサイズ</param>
	/// <param name="_map">マップチップ</param>
	static void SerializeBinary(const std::string& _fileName, std::vector<std::vector<std::vector<int>>> _map = { {{}} });

	/// <summary>
	/// 入力
	/// </summary>
	/// <param name="_fileName">ファイルの名前</param>
	/// <param name="_map">マップチップ</param>
	static bool DeserializeBinary(const std::string _fileName,std::vector<std::vector<std::vector<int>>>* _map);

private:

	static const std::string base_directory;
};