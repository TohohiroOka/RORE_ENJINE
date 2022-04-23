#include "FrictionObject.h"
#include "DirectInput.h"
#include "DebugText.h"

using namespace DirectX;

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

	SetPosition({ 0,0,-10 });
	SetScale({ 2,2,2 });

	return true;
}

void FrictionObject::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	//���Z�b�g
	if (input->PushKey(DIK_SPACE)) {
		position.x = 0.0f;
		movePower = 0.0f;
	}

	//�����x�̏㏸
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

	//����*�����x�̍��v
	float powor = mass * movePower;
	//�����C
	float dynamicFriction = powor - restFriction;
	//�Î~���C�����傫����Γ���
	if (dynamicFriction>0)
	{
		position.x += powor - restFriction;
	}

	//���ʓ��̕\��
	DebugText* text = DebugText::GetInstance();
	text->SetColor(1, 0, 0);
	std::string ST_restFriction = std::to_string(restFriction);
	std::string ST_powor = std::to_string(powor);
	text->Print("1 : " + ST_restFriction, 100, 50);
	text->Print("2 : " + ST_powor, 100, 100);

	text = nullptr;

	// �s��̍X�V�Ȃ�
	Object3d::Update();

	input = nullptr;
}