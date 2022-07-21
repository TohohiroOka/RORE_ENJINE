#include "Ground.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

std::unique_ptr<Ground> Ground::Create(const std::string heightmapFilename, const std::string filename)
{
	// オブジェクトのインスタンスを生成
	Ground* instance = new Ground();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化

	if (!instance->Initialize()) {
		delete instance;
		assert(0);
	}

	return std::unique_ptr<Ground>(instance);
}

bool Ground::Initialize()
{
	object = HeightMap::Create("heightmap01.bmp", "Dirt.jpg");

	// コライダーの追加
	MeshCollider* collider = new MeshCollider;
	object->SetCollider(collider);
	collider->ConstructTriangles(object->GetModel());
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	return true;
}

void Ground::Update()
{
	object->Update();
}

void Ground::Draw()
{
	object->Draw();
}
