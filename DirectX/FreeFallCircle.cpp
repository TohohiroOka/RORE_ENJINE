#include "FreeFallCircle.h"
#include "DirectInput.h"

using namespace DirectX;

FreeFallCircle* FreeFallCircle::Create(Model* model)
{
	// 3Dオブジェクトのインスタンスを生成
	FreeFallCircle* instance = new FreeFallCircle();
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

bool FreeFallCircle::Initialize()
{
	if (!Object3d::Initialize())
	{
		return false;
	}

	SetPosition({ 50,0,0 });
	SetColor({ 1,0,0,1 });
	SetScale({ 2,2,2 });

	return true;
}

void FreeFallCircle::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	//重力
	const float Gravity = 9.8f / 60.0f;

	if (input->PushKey(DIK_SPACE))
	{
		position.y = 100.0f;
		onGround = false;
	}

	if (!onGround)
	{
		addG += Gravity;
		position.y -= addG;
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