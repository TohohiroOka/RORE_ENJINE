#include "Cannonball.h"
#include "DirectInput.h"

using namespace DirectX;

Cannonball* Cannonball::Create(Model* model)
{
	// 3Dオブジェクトのインスタンスを生成
	Cannonball* instance = new Cannonball();
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

	return instance;
}

bool Cannonball::Initialize()
{
	if (!Object3d::Initialize())
	{
		return false;
	}

	Object3d::SetPosition({ 0,0,0 });
	Object3d::SetColor({ 0,0,1,1 });
	Object3d::SetScale({ 2,2,2 });

	return true;
}

void Cannonball::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	//重力
	const float Gravity = 9.8f / 60.0f;

	if (input->PushKey(DIK_SPACE))
	{
		position = { 0,0,0 };
		addG = -5.0f;
		onGround = false;
	}

	if (!onGround)
	{
		addG += Gravity;
		position.y -= addG;
		position.x += speed;
		if (position.y < 0)
		{
			onGround = true;
			position.y = 0;
			addG = 0;
		}
	}

	// 行列の更新など
	Object3d::Update();
}