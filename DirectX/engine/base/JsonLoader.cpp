#include <JsonLoader.h>

void JsonLoader::SerializeTest(const std::string& _fileName, const float _cameraDist,
    const std::array<int, 3> _mapSize, std::vector<std::vector<std::vector<int>>> _map)
{
    Person x;
    x.name = _fileName;
    x.cameraDist = 30.0f;
	x.mapSize = _mapSize;
	x.map = _map;
	// ファイル出力
    std::ofstream os(_fileName, std::ios::out);
    cereal::JSONOutputArchive archiveFile(os);
    x.serialize(archiveFile);

    // 標準出力
    std::stringstream ss;
    {
        cereal::JSONOutputArchive archiveStdout(ss);
        x.serialize(archiveStdout);
    }
    std::cout << ss.str() << std::endl;
}

bool JsonLoader::DeserializeTest(const std::string _fileName,
    float* _cameraDist, std::vector<std::vector<std::vector<int>>>* _map)
{
    Person x;

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