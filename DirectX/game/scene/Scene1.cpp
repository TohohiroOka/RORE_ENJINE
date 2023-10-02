#include "Scene1.h"
#include "Scene/SceneManager.h"
#include "Input/DirectInput.h"
#include "Input/XInputManager.h"
#include "Object/2d/DebugText.h"
#include "WindowApp.h"
#include <imgui.h>

using namespace DirectX;

const std::array<XMFLOAT4, 2> COLOR = { XMFLOAT4{ 0.0f,0.0f,0.8f,1.0f } ,{ 0.8f,0.0f,0.0f,1.0f } };

void Scene1::Initialize()
{
	camera = nullptr;

	Sprite::LoadTexture("amm","Resources/amm.jpg");
	sprite = Sprite::Create("amm");
	sprite->SetTexSize({ 1059.0f,1500.0f });
	sprite->SetSize({ 1059.0f / 5.0f,1500.0f / 5.0f });
	sprite->Update();

	Ground::StaticInitialize();

	object.reset(new GroundManager());

	player = std::make_unique<Player>();

	model=Model::CreateFromOBJ("NormalCube");
	obj = Object3d::Create(model.get());
	obj->SetScale({ 5.0f,5.0f, 5.0f });
}

void Scene1::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	obj->Update();
	player->Update();
	object->Update();
}

void Scene1::Draw(const int _cameraNum)
{
	obj->Draw();

	object->Draw();

	//object->ColliderDraw();

	player->Draw();
}

void Scene1::NonPostEffectDraw(const int _cameraNum)
{
	//�X�v���C�g
	if (_cameraNum == 0) {
		DebugText::GetInstance()->DrawAll();
		sprite->Draw();
	}
}

void Scene1::Finalize()
{
}

void Scene1::ImguiDraw()
{
	Vector3 ppos = player->GetPos();

	ImGui::Begin("debug imgui");
	ImGui::SetWindowSize(ImVec2(300, 300), ImGuiCond_::ImGuiCond_FirstUseEver);

	ImGui::Text("%f", cameraRota.x);
	ImGui::Text("%f : %f", sinf(XMConvertToRadians(cameraRota.x)), cosf(XMConvertToRadians(cameraRota.x)));
	ImGui::Text("%f : %f", sinf(XMConvertToRadians(cameraRota.x + 90)), cosf(XMConvertToRadians(cameraRota.x + 90)));
	ImGui::Text("%f : %f : %f", ppos.x, ppos.y, ppos.z);

	ImGui::End();
}

void Scene1::CameraUpdate(const int _cameraNum, Camera* _camera)
{
	const float camera_sa = 10.0f;
	const float LR_sa = 163.0f;
	const float UD_sa = 53.0f;

	//�ʏ�J����
	if (_cameraNum == 0) {
		DirectInput* input = DirectInput::GetInstance();
		//���E�ړ�
		const float Tgspeed = 1.0f;
		if (input->PushKey(DIK_LEFT)) { cameraRota.x -= Tgspeed; }//�E����
		if (input->PushKey(DIK_RIGHT)) { cameraRota.x += Tgspeed; }//������
		if (input->PushKey(DIK_DOWN)) { cameraRota.y -= Tgspeed; }//������
		if (input->PushKey(DIK_UP)) { cameraRota.y += Tgspeed; }//�����

		//�㉺�����̊p�x����
		if (cameraRota.y <= -13) { cameraRota.y = -13; }//������
		if (cameraRota.y >= 13) { cameraRota.y = 13; }//�㐧��

		//���̐���
		if (cameraRota.x > 360) { cameraRota.x = 0; }
		if (cameraRota.x < -360) { cameraRota.x = 0; }

		//�J�����ړ�
		float Pspeed = 1.0f;
		XMFLOAT2 radian = { XMConvertToRadians(cameraRota.x),XMConvertToRadians(cameraRota.y) };
		XMFLOAT2 radian2 = { XMConvertToRadians(cameraRota.x + 180),XMConvertToRadians(cameraRota.y + 180) };

		Vector3 ppos= player->GetPos();

		_camera->SetEye({ ppos.x + sinf(radian2.x) * 10.0f,ppos.y + sinf(radian2.y) * 10.0f,ppos.z + cosf(radian2.x) * 10.0f });
		_camera->SetTarget({ ppos.x + sinf(radian.x) * 10.0f,ppos.y + sinf(radian.y) * 10.0f,ppos.z + cosf(radian.x) * 10.0f });

		player->SetMoveRota(cameraRota.x);

		camera = _camera;
	}
}

void Scene1::FrameReset()
{
}