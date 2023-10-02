#include "Ground.h"
#include "Object/3d/collider/Collision.h"
#include "Object/3d/collider/MeshCollider.h"
#include "Object/3d/collider/CollisionAttribute.h"

std::array<std::shared_ptr<TerrainModel>, groundModelNum* int(GroundType::size)> Ground::model;

const float distPos = 255.0f * groundSize;
const std::array<Vector3, int(GroundType::size)> surfacePos = {
	Vector3{0.0f,0.0f,0.0f},{0.0f,distPos,distPos},{0.0f,distPos,0.0f},{0.0f,0.0f,distPos},{distPos,0.0f,0.0f},{0.0f,distPos,0.0f}};
const std::array<Vector3, int(GroundType::size)> surfaceRota = {
	Vector3{0.0f,0.0f,0.0f},{180.0f,0.0f,0.0f},{90.0f,0,0.0f},{-90.0f,0.0f,0.0f},{0.0f,0.0f,90.0f},{0.0f,0.0f,-90.0f}};

Ground::Ground(const int type)
{
	object = HeightMap::Create(model[int(type)].get());
	object->SetScale({ groundSize ,groundSize ,groundSize });
	object->SetPosition(surfacePos[type]);
	object->SetRotation(surfaceRota[type]);
	object->UpdateWorldMatrix();

	object->DeleteCollider();

	// コライダーの追加
	MeshCollider* collider = new MeshCollider;
	object->SetCollider(collider);
	collider->ConstructTriangles(object->GetModel());
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);

}

void Ground::StaticInitialize()
{
	const std::string jimen = "jimen.png";
	const std::string kabe = "kabe.png";


	//モデル読み込み
	int count=0;
	for (int i = 0; i < groundModelNum; i++) {
		for (int type = 0; type< int(GroundType::size); type++) {
			model[count] = TerrainModel::Create("heightmap" + std::to_string(i) + ".bmp", 5.0f,
				{ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, 1.0f, jimen, kabe);
			count++;
		}
	}
}

void Ground::Initialize()
{
}

void Ground::Update()
{
	object->Update();
}

void Ground::Draw()
{
	//object->Draw();

	object->ColliderDraw();
}
