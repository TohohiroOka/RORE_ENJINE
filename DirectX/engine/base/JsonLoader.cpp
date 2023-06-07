#include <JsonLoader.h>
#include <json.hpp>

using njson = nlohmann::json;
const std::string JsonLoader::base_directory = "Resources/";

void JsonLoader::LoadShaderName(const std::string& _fileName, std::vector<Shader>* _shaderName)
{
    const std::string fullpath = base_directory + "Shaders/" + _fileName + ".json";

    //ファイルストリーム
    std::ifstream file;

    //ファイルを開く
    file.open(fullpath);
    if (file.fail()) {
        assert(0);
    }

    //解凍データ保存先
    njson deserialized;

    //解凍
    file >> deserialized;

    //データが正しいかのチェック
    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].is_string());

    //"name"を文字列として取得
    std::string name = deserialized["name"].get<std::string>();
    //正しいレベルデータファイルかチェック
    assert(name.compare("scene") == 0);

    for (njson& itr : deserialized["shaders"])
    {
        njson& data = itr["shader"];

        Shader add{};
        add.name = data["Name"];
        for (auto& i : data["Type"]) {
            add.type.emplace_back(i);
        }
        _shaderName->emplace_back(add);
    }
}


void JsonLoader::LoadPipeline(const std::string& _fileName, std::vector<Pipeline>* _pipeline)
{
    const std::string fullpath = base_directory + "Pipeline/" + _fileName + ".json";

    //ファイルストリーム
    std::ifstream file;

    //ファイルを開く
    file.open(fullpath);
    if (file.fail()) {
        assert(0);
    }

    //解凍データ保存先
    njson deserialized;

    //解凍
    file >> deserialized;

    //データが正しいかのチェック
    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].is_string());

    //"name"を文字列として取得
    std::string name = deserialized["name"].get<std::string>();
    //正しいレベルデータファイルかチェック
    assert(name.compare("scene") == 0);

    for (njson& itr : deserialized["pipelines"])
    {
        Pipeline addPipe{};
        //トランスフォーム情報
        njson& data = itr["pipeline"];
        addPipe.name = data["Name"];
        for (auto& i : data["ShaderType"]) {
            addPipe.shaderType.emplace_back(i);
        }
        for (auto& i : data["InputLayoutType"]) {
            addPipe.inputLayoutType.emplace_back(i);
        }
        addPipe.rtvNum = data["RtvNum"];
        addPipe.blendMode = data["BlendMode"];
        addPipe.drawMode = data["DrawMode"];
        addPipe.drawType = data["DrawType"];
        addPipe.textureNum = data["TextureNum"];
        addPipe.rootparams = data["Rootparams"];

        _pipeline->emplace_back(addPipe);
    }
}

void JsonLoader::SerializeJsonMap(const std::string& _fileName, const float _cameraDist,
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

bool JsonLoader::DeserializeJsonMap(const std::string _fileName,
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

void JsonLoader::SerializeBinary(const std::string& _fileName, const bool _is2D, const int _moveSurface, const std::array<int, 3>& _mapChip,
    const std::array<float, 3>& _cameraPos, const std::array<int, 2>& _cameraPosPhase, const std::array<float, 3>& _cameraRota, const std::array<float, 3>& _playerPos)
{
    Binary x;
    x.is2D = int(_is2D);
    x.moveSurface = _moveSurface;
    x.mapChip = _mapChip;
    x.cameraPos = _cameraPos;
    x.cameraPosPhase = _cameraPosPhase;
    x.cameraRota = _cameraRota;
    x.playerPos = _playerPos;

    // ファイル出力
    std::ofstream os("Resources/binary/" + _fileName + ".binary", std::ios::out | std::ios::binary);
    cereal::BinaryOutputArchive archive(os);
    x.serialize(archive);
}

bool JsonLoader::DeserializeBinary(const std::string& _fileName, bool* _is2D, int* _moveSurface, std::array<int, 3>* _mapChip,
    std::array<float, 3>* _cameraPos, std::array<int, 2>* _cameraPosPhase, std::array<float, 3>* _cameraRota, std::array<float, 3>* _playerPos)
{
    Binary x;

    std::ifstream os("Resources/binary/" + _fileName + ".binary", std::ios::in | std::ios::binary);
    if (!os.is_open()) {
        return false;
    }
    cereal::BinaryInputArchive archive(os);
    x.serialize(archive);

    *_is2D = bool(x.is2D);
    *_moveSurface = x.moveSurface;
    *_mapChip = x.mapChip;
    *_cameraPos = x.cameraPos;
    *_cameraPosPhase = x.cameraPosPhase;
    *_cameraRota = x.cameraRota;
    *_playerPos = x.playerPos;

    return true;
}
