#pragma once
#include "Object/3d/HeightMap.h"
#include "GroundiInformation.h"
#include "Math/Vector3.h"

class Ground
{

public:

	Ground(const int type);
	~Ground(){};
	
	static void StaticInitialize();

	void Initialize();

	void Update();

	void Draw();

private:

	//���f��
	static std::array<std::shared_ptr<TerrainModel>, groundModelNum * int(GroundType::size)> model;
	//�I�u�W�F�N�g
	std::shared_ptr<HeightMap> object;
	//���W
	Vector3 pos;
};

