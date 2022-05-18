#include "FrictionObject.h"
#include "DirectInput.h"
#include "DebugText.h"

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

	SetScale({ 2,2,2 });

	return true;
}

void FrictionObject::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	//
	if (input->PushKey(DIK_M))
	{
		movePower += muki * 0.005f;
	} else if (!input->PushKey(DIK_M) && movePower != 0)
	{
		movePower += muki * -1 * 0.001f;
		if (movePower < 0.02f&& movePower >= -0.02f) {
			movePower = 0;
		}
	}

	//現在のF
	float powor = mass * movePower;
	//動摩擦-静止摩擦
	float dynamicFriction = abs(powor) - restFriction;
	if (dynamicFriction > 0)
	{
		int a = 1;
		if (movePower < 0) { a = -1; }
		position.x += dynamicFriction * a;
	}

	if (powor > 1)
	{
		DebugText* text = DebugText::GetInstance();
		text->SetColor({ 1, 0, 0, 0 });
		std::string ST_restFriction = std::to_string(powor);
		std::string ST_powor = std::to_string(dynamicFriction);
		text->Print("1 : " + ST_restFriction, { 100, 50 });
		text->Print("2 : " + ST_powor, { 100, 100 });
	}

	// 行列の更新など
	Object3d::Update();

	input = nullptr;

}
