#include "Scene1.h"
#include "SceneManager.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"
#include "WindowApp.h"
#include <imgui.h>
#include "Collision.h"

using namespace DirectX;

void Scene1::Initialize()
{
	camera = nullptr;

	cameraPos = { 0,0,-10 };
	cameraTarget = { 135,0,0 };
}

void Scene1::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	//DebugText* text = DebugText::GetInstance();
	//text->Print("export", 100, 100, { 1.0f,1.0f ,1.0f }, 20);
}

void Scene1::Draw(const int _cameraNum)
{
}

void Scene1::Finalize()
{
}

void Scene1::ImguiDraw()
{
	ImGui::Begin("debug imgui");
	ImGui::SetWindowSize(ImVec2(300, 300), ImGuiCond_::ImGuiCond_FirstUseEver);

	ImGui::Text("Map Change Direction");
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
		if (input->PushKey(DIK_RIGHT)) { cameraTarget.x -= Tgspeed; }//�E����
		if (input->PushKey(DIK_LEFT)) { cameraTarget.x += Tgspeed; }//������
		if (input->PushKey(DIK_DOWN)) { cameraTarget.y -= 0.01f + abs(cameraTarget.y) * 0.01f; }//������
		if (input->PushKey(DIK_UP)) { cameraTarget.y += 0.01f + abs(cameraTarget.y) * 0.01f; }//�����

		//�㉺�����̊p�x����
		if (cameraTarget.y <= -13) { cameraTarget.y = -13; }//������
		if (cameraTarget.y >= 13) { cameraTarget.y = 13; }//�㐧��

		//���̐���
		if (cameraTarget.x > 360) { cameraTarget.x = 0; }
		if (cameraTarget.x < -360) { cameraTarget.x = 0; }

		//�J�����ړ�
		float Pspeed = 1.0f;
		XMFLOAT2 radian = { XMConvertToRadians(cameraTarget.x + 90),XMConvertToRadians(cameraTarget.x) };

		//�E����
		if (input->PushKey(DIK_S)) {
			cameraPos.x += Pspeed * cos(radian.x);
			cameraPos.z += Pspeed * sin(radian.x);
		}
		//������
		if (input->PushKey(DIK_W)) {
			cameraPos.x -= Pspeed * cos(radian.x);
			cameraPos.z -= Pspeed * sin(radian.x);
		}
		//������
		if (input->PushKey(DIK_D)) {
			cameraPos.x -= Pspeed * cos(radian.y);
			cameraPos.z -= Pspeed * sin(radian.y);
		}
		//�����
		if (input->PushKey(DIK_A)) {
			cameraPos.x += Pspeed * cos(radian.y);
			cameraPos.z += Pspeed * sin(radian.y);
		}
		//������
		if (input->PushKey(DIK_SPACE)) {
			cameraPos.y += Pspeed;
		}
		//�����
		if (input->PushKey(DIK_LSHIFT)) {
			cameraPos.y -= Pspeed;
		}

		_camera->SetEye(cameraPos);
		_camera->SetTarget({ cameraPos.x - cosf(radian.x),cameraPos.y + cameraTarget.y ,cameraPos.z - sinf(radian.x) });

		camera = _camera;
	}
}

void Scene1::FrameReset()
{
}