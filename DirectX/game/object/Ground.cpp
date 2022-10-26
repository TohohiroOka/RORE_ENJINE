﻿#include "Ground.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

 const float Ground::scale = 8.0f;

std::unique_ptr<Ground> Ground::Create(const std::string& heightmapFilename,
	const std::string& filename1, const std::string& filename2)
{
	// オブジェクトのインスタンスを生成
	Ground* instance = new Ground();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->object = HeightMap::Create(heightmapFilename, filename1, filename2);
	instance->Initialize();

	return std::unique_ptr<Ground>(instance);
}

void Ground::Initialize()
{
	object->SetScale({ scale,scale,scale });

	// コライダーの追加
	MeshCollider* collider = new MeshCollider;
	object->SetCollider(collider);
	//collider->ConstructTriangles(object->GetModel());
	collider->ConstructTriangles(object->GetHitVertices(), object->GetHitIndices());
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);
}

void Ground::Update()
{
	object->Update();
}

void Ground::Draw()
{
	object->Draw();
}

void Ground::CDraw()
{
	object->ColliderDraw();
}

void Ground::SetPos(DirectX::XMFLOAT3 pos)
{
	object->SetPosition(pos);
}