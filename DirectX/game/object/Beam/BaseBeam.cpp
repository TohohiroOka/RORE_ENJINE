#include "BaseBeam.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "GameHelper.h"

using namespace DirectX;

std::unique_ptr<Model> BaseBeam::model;
std::array<std::unique_ptr<InstanceObject>, BaseBeam::object_max_num> BaseBeam::object;

void BaseBeam::StaticInitialize()
{
	//’e
	model = Model::CreateFromOBJ("Square1");

	for (auto& i : object)
	{
		i = InstanceObject::Create(model.get());
		i->SetBloom(true);
		i->SetLight(false);
	}
}

void BaseBeam::Initialize()
{
	isAlive = true;
	float inSize = mapSize / 700.0f;
	scale = { inSize,inSize,mapSize / 2.0f };
}

void BaseBeam::Update()
{
	pos.x += move.x;
	pos.y += move.y;
	pos.z += move.z;

	for (auto& i : object)
	{
		if (!i->GetInstanceDrawCheck()) { continue; }
		i->DrawInstance(pos, scale, rotate, color);
		return;
	}
}

void BaseBeam::Draw()
{
	for (auto& i : object) {
		if (i->GetInstanceDrawNum() == 0) { continue; }
		i->Draw();
	}
}

void BaseBeam::Finalize()
{
	model.reset();
}