#include <JsonLoader.h>

void JsonLoader::SerializeJson(const std::string& _fileName, const float _cameraDist,
    const std::array<int, 3> _mapSize, std::vector<std::vector<std::vector<int>>> _map)
{
    Json x;
    x.name = _fileName;
    x.cameraDist = 30.0f;
	x.mapSize = _mapSize;
	x.map = _map;
	// ファイル出力
    std::ofstream os(_fileName, std::ios::out);
    cereal::JSONOutputArchive archiveFile(os);
    x.serialize(archiveFile);
}

bool JsonLoader::DeserializeJson(const std::string _fileName,
    float* _cameraDist, std::vector<std::vector<std::vector<int>>>* _map)
{
    Json x;

    std::ifstream os(_fileName, std::ios::in);
    if (!os.is_open()) {
        return false;
    }
    cereal::JSONInputArchive archive(os);
    x.serialize(archive);

    *_cameraDist = x.cameraDist;
    *_map = x.map;

    return true;
}

void JsonLoader::SerializeBinary(const std::string& _fileName,std::vector<std::vector<std::vector<int>>> _map)
{
    Binary x;
    x.map = _map;

    // ファイル出力
    std::ofstream os("binary/" + _fileName + ".binary", std::ios::out | std::ios::binary);
    cereal::BinaryOutputArchive archive(os);
    x.serialize(archive);
}

bool JsonLoader::DeserializeBinary(const std::string _fileName, std::vector<std::vector<std::vector<int>>>* _map)
{
    Binary x;

    std::ifstream os("binary/" + _fileName +".binary", std::ios::in | std::ios::binary);
    if (!os.is_open()) {
        return false;
    }
    cereal::BinaryInputArchive archive(os);
    x.serialize(archive);

    *_map = x.map;

    return true;
}
