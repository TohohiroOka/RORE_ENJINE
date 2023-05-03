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

class JsonLoder
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
	/// �ǂݍ���
	/// </summary>
	/// <param name="_fileName">�t�@�C���̖��O</param>
	/// <param name="_delimitNum">��؂�̐�</param>
	static void SerializeTest(const std::string& _fileName = "out.json", const float _cameraDist = 30.0f,
		const std::array<int, 3> _mapSize = { 10,10,10 }, std::vector<std::vector<std::vector<int>>> _map = { {{}} });

	/// <summary>
	/// json�̏�������
	/// </summary>
	static void DeserializeTest();
};