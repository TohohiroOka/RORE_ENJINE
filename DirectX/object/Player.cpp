#include "Player.h"
#include "DirectInput.h"
#include "DebugText.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

using namespace DirectX;

std::unique_ptr<Player> Player::Create(Model * model)
{
	// 3Dオブジェクトのインスタンスを生成
	Player* instance = new Player();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
	}

	if (model) {
		instance->SetModel(model);
	}

	return std::unique_ptr<Player>(instance);
}

bool Player::Initialize()
{
	if (!Object3d::Initialize())
	{
		return false;
	}

	// コライダーの追加
	float radius = 0.6f;
	SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	collider->SetAttribute(COLLISION_ATTR_ALLIES);

	//SetBloom(true);

	SetOutline(true);
	SetOutlineWidth(0.002f);
	SetOutlineColor({ 1,1,1,1 });

	SetPosition({ 0,0,-100 });

	//SetToon(true);
	SetScale({ 2,2,2 });

	return true;
}

void Player::Update()
{
	DirectInput* input = DirectInput::GetInstance();
	// 行列の更新など
	Object3d::Update();

	input = nullptr;
}