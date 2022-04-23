#include "FrictionObject.h"
#include "DirectInput.h"
#include "DebugText.h"

using namespace DirectX;

std::unique_ptr<FrictionObject> FrictionObject::Create(Model* model)
{
	// 3Dオブジェクトのインスタンスを生成
	FrictionObject* instance = new FrictionObject();
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

	return std::unique_ptr<FrictionObject>(instance);
}

bool FrictionObject::Initialize()
{
	if (!Object3d::Initialize())
	{
		return false;
	}

	SetPosition({ 0,0,-10 });
	SetScale({ 2,2,2 });

	return true;
}

void FrictionObject::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	//リセット
	if (input->PushKey(DIK_SPACE)) {
		position.x = 0.0f;
		movePower = 0.0f;
	}

	//加速度の上昇
	if (input->PushKey(DIK_RIGHT))
	{
		movePower += 0.005f;
	}
	else if (!input->PushKey(DIK_RIGHT) && movePower > 0)
	{
		movePower -= 0.001f;
		if (movePower <= 0) {
			movePower = 0;
		}
	}

	//質量*加速度の合計
	float powor = mass * movePower;
	//動摩擦
	float dynamicFriction = powor - restFriction;
	//静止摩擦よりも大きければ動く
	if (dynamicFriction>0)
	{
		position.x += powor - restFriction;
	}

	//質量等の表示
	DebugText* text = DebugText::GetInstance();
	text->SetColor(1, 0, 0);
	std::string ST_restFriction = std::to_string(restFriction);
	std::string ST_powor = std::to_string(powor);
	text->Print("1 : " + ST_restFriction, 100, 50);
	text->Print("2 : " + ST_powor, 100, 100);

	text = nullptr;

	// 行列の更新など
	Object3d::Update();

	input = nullptr;
}