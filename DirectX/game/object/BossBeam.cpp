#include "BossBeam.h"

using namespace DirectX;

std::unique_ptr<Model> BossBeam::model;

void BossBeam::StaticInitialize()
{
	model = Model::CreateFromOBJ("Square");//プレイヤーの弾
}

std::unique_ptr<BossBeam> BossBeam::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	BossBeam* instance = new BossBeam();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();

	return std::unique_ptr<BossBeam>(instance);
}

void BossBeam::Initialize()
{
	isAlive = true;
	scale = 1000;
	angle = { 0.0f,0.0f };

	object = Object3d::Create(model.get());
	object->SetBloom(true);
	object->SetScale({ 10.0f,10.0f ,scale });
	object->SetRotation({ 0,angle.x,angle.y });
	object->Update();
}

void BossBeam::Update(const XMFLOAT3& _pos, const XMFLOAT3& _color)
{
	isAlive = true;
	angle.x += 5.0f;
	angle.y += 1.0f;

	float radiunZY = XMConvertToRadians(angle.x);
	float radiunXZ = XMConvertToRadians(angle.y);

	pos = { _pos.x + cos(radiunZY) * sin(radiunXZ) * scale * 1.01f,
		_pos.y - sin(radiunZY) * scale * 1.01f,
		_pos.z + cos(radiunZY) * cos(radiunXZ) * scale * 1.01f };

	object->SetColor({ 1.0f,1.0f,1.0f,0.1f });
	object->SetPosition(pos);
	object->SetRotation({ angle.x,angle.y,0 });
	object->Update();
}

void BossBeam::Draw()
{
	if (!isAlive) { return; }
	object->Draw();
}

void BossBeam::Finalize()
{
	model.reset();
}