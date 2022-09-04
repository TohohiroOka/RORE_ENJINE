#include "PlayerBullet.h"

PlayerBullet::~PlayerBullet()
{
}

std::unique_ptr<PlayerBullet> PlayerBullet::Create()
{
	//インスタンスを生成
	PlayerBullet* instance = new PlayerBullet();
	if (instance == nullptr) {
		return nullptr;
	}

	//初期化
	instance->Initialize();

	return std::unique_ptr<PlayerBullet>(instance);
}

void PlayerBullet::Initialize()
{
	BaseBullet::Initialize();

	object.resize(bulletNum);
	for (int i = 0; i < bulletNum; i++)
	{
		object[i].instance = Object3d::Create(model.get());
		object[i].instance->SetScale({ 1,1,1 });
	}
}

void PlayerBullet::Update()
{
	for (auto& itr : object)
	{
		if (!itr.isAlive) { continue; }

		XMFLOAT3 position = itr.instance->GetPosition();

		position.x += itr.vecMove.x;
		position.y += itr.vecMove.y;
		position.z += itr.vecMove.z;

		itr.instance->SetPosition(position);
		itr.instance->Update();
	}
}

void PlayerBullet::Draw()
{
	for (auto& itr : object)
	{
		if (!itr.isAlive) { continue; }
		itr.instance->Draw();
	}
}

void PlayerBullet::SetMove(const XMFLOAT3 _pos, const XMFLOAT3 _vecMove)
{
	for (auto& itr : object)
	{
		if (itr.isAlive) { continue; }
		const float speed = 50.0f;
		itr.instance->SetPosition(_pos);
		itr.vecMove.x = _vecMove.x * speed;
		itr.vecMove.y = _vecMove.y * speed;
		itr.vecMove.z = _vecMove.z * speed;
		itr.isAlive = true;
		return;
	}
}

void PlayerBullet::Reset()
{
	for (auto& itr : object)
	{
		itr.instance->SetPosition({ 0,0,0 });
		itr.vecMove = { 0,0,0 };
		itr.isAlive = false;;
	}
}
