#include "FrictionObject.h"
#include "DirectInput.h"
#include "DebugText.h"

std::unique_ptr<FrictionObject> FrictionObject::Create(Model* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	FrictionObject* instance = new FrictionObject();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
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

	//���݂�F
	float powor = mass * movePower;
	//�����C-�Î~���C
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

	// �s��̍X�V�Ȃ�
	Object3d::Update();

	input = nullptr;

}
