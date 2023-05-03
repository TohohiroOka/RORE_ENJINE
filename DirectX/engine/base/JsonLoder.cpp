#include <JsonLoder.h>

void JsonLoder::SerializeTest(const std::string& _fileName, const float _cameraDist,
    const std::array<int, 3> _mapSize, std::vector<std::vector<std::vector<int>>> _map)
{
	std::vector<std::vector<std::vector<int>>> mapadd = {
	{
	{1,2,3,4,5},
	{6,7,8,9,10},
	{11,12,13,14,15},
	{16,17,18,19,20},
	{21,22,23,24,25}
	},
	{
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1}
	},
	{
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1}
	},
	{
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1}
	},
	{
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1}
	} };

    Person x;
    x.name = "map1";
    x.cameraDist = 30.0f;
	x.mapSize = _mapSize;
	x.map = mapadd;
	// ファイル出力
    std::ofstream os("out.json", std::ios::out);
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

void JsonLoder::DeserializeTest()
{
    //Person person;

    //std::ifstream os("out.json", std::ios::in);
    //cereal::JSONInputArchive archive(os);
    //person.serialize(archive);

    //std::cout << person.name << std::endl;
    //std::cout << person.cameraDist << std::endl;
    //std::cout << person.mapSize[0] << person.mapSize[1] << person.mapSize[2] << std::endl;

    //for (int y = 0; y < person.mapSize[1]; y++) {
    //    for (int z = 0; z < person.mapSize[2]; z++) {
    //        for (int x = 0; x < person.mapSize[0]; x++) {
    //            std::cout << person.map[y][x][z];

    //        }
    //        std::cout << std::endl;
    //    }
    //}
}