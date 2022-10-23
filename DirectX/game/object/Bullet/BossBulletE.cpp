#include "BossBulletE.h"
#include "BulletManager.h"
#include "GameHelper.h"

using namespace DirectX;

BossBulletE::BossBulletE(const XMFLOAT3& _pos)
{
	pos = _pos;
	move = { 0,0,0 };
	timer = 0;
	isMove = false;
}

std::unique_ptr<BossBulletE> BossBulletE::Create(const XMFLOAT3& _pos, const XMFLOAT3& _color)
{
	// 3Dオブジェクトのインスタンスを生成
	BossBulletE* instance = new BossBulletE(_pos);
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();
	instance->object->SetColor({ _color.x,_color.y,_color.z,1.0f });

	return std::unique_ptr<BossBulletE>(instance);
}

void BossBulletE::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();
}

void BossBulletE::Update()
{
	timer++;

	BaseBullet::Update();

	//移動開始
	if (timer > 100&& !isMove){
		isMove = true;
		moveVec = {
			Randomfloat(100) / 100.0f * RandomSign(),
			Randomfloat(100) / 100.0f * RandomSign(),
			Randomfloat(100) / 100.0f * RandomSign() };

		const float speed = 15.0f;
		move = { moveVec.x * speed,moveVec.y * speed,moveVec.z * speed };
	}
}