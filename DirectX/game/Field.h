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

	//���f��
	std::shared_ptr<TerrainModel> groundModel;
	//�I�u�W�F�N�g
	std::shared_ptr<HeightMap> groundObject;

	//�O�n�`

};

