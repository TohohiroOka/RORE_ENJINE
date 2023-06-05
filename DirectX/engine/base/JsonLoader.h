#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
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
		float cameraDist;
		std::array<int, 3> mapSize;
		std::vector<std::vector<std::vector<int>>> map;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(cameraDist), CEREAL_NVP(mapSize), CEREAL_NVP(map));
		}
	};

	struct Binary
	{
		int is2D;
		int moveSurface;
		std::array<int, 3> mapChip;
		std::array<float, 3> cameraPos;
		std::array<int, 2> cameraPosPhase;
		std::array<float, 3> cameraRota;
		std::array<float, 3> playerPos;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(CEREAL_NVP(is2D), CEREAL_NVP(moveSurface), CEREAL_NVP(mapChip),
				CEREAL_NVP(cameraPos), CEREAL_NVP(cameraPosPhase), CEREAL_NVP(cameraRota), CEREAL_NVP(playerPos));
		}
	};

public:

	/// <summary>
	/// jsonの読み込み
	/// </summary>
	/// <param name="_fileName">ファイル名</param>
	/// <param name="_name"></param>
	/// <param name="_inputLayoutType"></param>
	/// <param name="_rtvNum"></param>
	/// <param name="_blendMode"></param>
	/// <param name="_rootparams"></param>
	static void LoadPipelineNlohmannJson(const std::string& _fileName, std::string& _name,
		std::vector<std::string>* _inputLayoutType, int* _rtvNum, std::string& _blendMode, int* _rootparams);

	/// <summary>
	/// 出力
	/// </summary>
	/// <param name="_fileName">ファイルの名前</param>
	/// <param name="_cameraDist">カメラの距離</param>
	/// <param name="_mapSize">マップサイズ</param>
	/// <param name="_map">マップチップ</param>
	static void SerializeJsonMap(const std::string& _fileName = "out.json", const float _cameraDist = 30.0f,
		const std::array<int, 3> _mapSize = { 10,10,10 }, std::vector<std::vector<std::vector<int>>> _map = { {{}} });

	/// <summary>
	/// 入力
	/// </summary>
	/// <param name="_fileName">ファイルの名前</param>
	/// <param name="_cameraDist">カメラの距離</param>
	/// <param name="_map">マップチップ</param>
	static bool DeserializeJsonMap(const std::string _fileName, float* _cameraDist,
		std::vector<std::vector<std::vector<int>>>* _map);

	/// <summary>
	/// 出力
	/// </summary>
	/// <param name="_fileName">ファイルの名前</param>
	/// <param name="_is2D">2Dかどうか</param>
	/// <param name="_moveSurface">移動面</param>
	/// <param name="_mapChip">プレイヤーマップチップ</param>
	/// <param name="_cameraPos">カメラ座標</param>
	/// <param name="_cameraPosPhase">カメラの方向</param>
	/// <param name="_cameraRota">カメラ角度</param>
	/// <param name="_playerPos">プレイヤー座標</param>
	static void SerializeBinary(const std::string& _fileName, const bool _is2D, const int _moveSurface,
		const std::array<int, 3>& _mapChip, const std::array<float, 3>& _cameraPos, const std::array<int, 2>& _cameraPosPhase,
		const std::array<float, 3>& _cameraRota, const std::array<float, 3>& _playerPos);

	/// <summary>
	/// 入力
	/// </summary>
	/// <param name="_fileName">ファイルの名前</param>
	/// <param name="_is2D">2Dかどうか</param>
	/// <param name="_moveSurface">移動面</param>
	/// <param name="_mapChip">プレイヤーマップチップ</param>
	/// <param name="_cameraPos">カメラ座標</param>
	/// <param name="_cameraPosPhase">カメラの方向</param>
	/// <param name="_cameraRota">カメラ角度</param>
	/// <param name="_playerPos">プレイヤー座標</param>
	static bool DeserializeBinary(const std::string& _fileName, bool* _is2D, int* _moveSurface,
		std::array<int, 3>* _mapChip, std::array<float, 3>* _cameraPos, std::array<int, 2>* _cameraPosPhase,
		std::array<float, 3>* _cameraRota, std::array<float, 3>* _playerPos);

private:

	static const std::string base_directory;
};