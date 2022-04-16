#include "Cannonball.h"
#include "DirectInput.h"

using namespace DirectX;

Cannonball* Cannonball::Create(Model* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Cannonball* instance = new Cannonball();
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

	//�d��
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

	// �s��̍X�V�Ȃ�
	Object3d::Update();
}