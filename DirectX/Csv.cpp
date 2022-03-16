#include "Csv.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void Csv::LoadFile(string fileName, vector<vector<int>> map)
{
	std::ifstream ifs(fileName);
	std::string line;
	int y = 0;

	while (getline(ifs, line))
	{
		std::istringstream stream(line);
		std::string field;

		int addY = y + 1;

		map.resize(addY);

		while (std::getline(stream, field, ','))
		{
			map[y].push_back(stoi(field));
		}

		//ŽŸ‚Ì’i‚É‚¢‚­
		y += 1;
	}
}

int Csv::GetChipNum(vector<vector<int>> map, float chipSize, XMFLOAT2 position)
{
	int X = position.x / chipSize;
	int Y = position.y / chipSize;

	int maxX = map[0].size();
	int maxY = map.size();

	if (X < 0 || X >= maxX || Y < 0 || Y >= maxY)
	{
		return 0;
	}

	return map[Y][X];
}