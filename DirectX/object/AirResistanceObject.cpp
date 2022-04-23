#include "AirResistanceObject.h"
#include "DirectInput.h"
#include "DebugText.h"

using namespace DirectX;

std::unique_ptr<AirResistanceObject> AirResistanceObject::Create(Model* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	AirResistanceObject* instance = new AirResistanceObject();
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

	return std::unique_ptr<AirResistanceObject>(instance);
}

bool AirResistanceObject::Initialize()
{
	if (!Object3d::Initialize())
	{
		return false;
	}

	SetPosition({ 0,0,-10 });
	SetScale({ 2,2,2 });

	return true;
}

void AirResistanceObject::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	//���Z�b�g
	if (input->PushKey(DIK_SPACE)) {
		position.x = 0.0f;
		position.y = 10.0f;
		addG = -5.0f;
		accel = 0.0f;
		speed = 2.0f;
	}

	//����
	if (input->PushKey(DIK_RETURN))
	{
		position = { 0,10,-20 };
		addG = -5.0f;
		accel = 0.0f;
		speed = 2.0f;
		onGround = false;
	}

	if (!onGround)
	{
		addG += gravity - (k * addG) / 60.0f;
		accel -= (k * accel) / 60.0f;
		speed += accel;
		position.y -= addG;
		position.x += speed;

		//�n�ʂɒ�������
		if (position.y <= 10.0f)
		{
			onGround = true;
			position.y = 10.0f;
			addG = -5.0f;
			accel = 0.0f;
			speed = 2.0f;
		}
	}


	//���ʓ��̕\��
	DebugText* text = DebugText::GetInstance();
	text->SetColor(1, 0, 0);
	std::string ST_restFriction = std::to_string(speed);
	std::string ST_powor = std::to_string(addG);
	text->Print("1 : " + ST_restFriction, 100, 150);
	text->Print("2 : " + ST_powor, 100, 200);

	text = nullptr;

	// �s��̍X�V�Ȃ�
	Object3d::Update();

	input = nullptr;
}