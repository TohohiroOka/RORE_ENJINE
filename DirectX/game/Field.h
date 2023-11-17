#pragma once
#include "Object/3d/HeightMap.h"

class Field
{
public:
	Field();
	~Field(){};

	void Update();

	void Draw();


private:

	//モデル
	std::shared_ptr<TerrainModel> groundModel;
	//オブジェクト
	std::shared_ptr<HeightMap> groundObject;

	//外地形

};

