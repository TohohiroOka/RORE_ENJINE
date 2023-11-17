#include "Field.h"
#include "Object/3d/collider/Collision.h"
#include "Object/3d/collider/MeshCollider.h"
#include "Object/3d/collider/CollisionAttribute.h"

Field::Field()
{
	const std::string jimen = "jimen.png";
	const std::string kabe = "kabe.png";
	groundModel = TerrainModel::Create("heightmap3.bmp", 30.0f,
		{ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, 1.0f, jimen, kabe);

	groundObject = HeightMap::Create(groundModel.get());
	groundObject->SetScale({ 2.0f ,2.0f ,2.0f });
	groundObject->UpdateWorldMatrix();

	groundObject->DeleteCollider();

	// コライダーの追加
	MeshCollider* collider = new MeshCollider;
	groundObject->SetCollider(collider);
	collider->ConstructTriangles(groundObject->GetModel());
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);
}

void Field::Update()
{
	groundObject->Update();
}

void Field::Draw()
{
	groundObject->Draw();
}
