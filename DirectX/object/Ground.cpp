#include "Ground.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

std::unique_ptr<Ground> Ground::Create(Model * model)
{
	// オブジェクトのインスタンスを生成
	Ground* instance = new Ground();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize(model)) {
		delete instance;
		assert(0);
	}

	instance->SetScale(100);
	instance->SetPosition({ 1, -5, 0 });

	return std::unique_ptr<Ground>(instance);
}

bool Ground::Initialize(Model * model)
{
	if (!Object3d::Initialize())
	{
		return false;
	}

	SetModel(model);

	// コライダーの追加
	MeshCollider* collider = new MeshCollider;
	SetCollider(collider);
	collider->ConstructTriangles(model);
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	return true;
}
