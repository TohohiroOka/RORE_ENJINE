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

	//通常カメラ
	if (_cameraNum == 0) {
		DirectInput* input = DirectInput::GetInstance();
		//視界移動
		const float Tgspeed = 1.0f;
		if (input->PushKey(DIK_RIGHT)) { cameraTarget.x -= Tgspeed; }//右入力
		if (input->PushKey(DIK_LEFT)) { cameraTarget.x += Tgspeed; }//左入力
		if (input->PushKey(DIK_DOWN)) { cameraTarget.y -= 0.01f + abs(cameraTarget.y) * 0.01f; }//下入力
		if (input->PushKey(DIK_UP)) { cameraTarget.y += 0.01f + abs(cameraTarget.y) * 0.01f; }//上入力

		//上下方向の角度制限
		if (cameraTarget.y <= -13) { cameraTarget.y = -13; }//下制限
		if (cameraTarget.y >= 13) { cameraTarget.y = 13; }//上制限

		//横の制限
		if (cameraTarget.x > 360) { cameraTarget.x = 0; }
		if (cameraTarget.x < -360) { cameraTarget.x = 0; }

		//カメラ移動
		float Pspeed = 1.0f;
		XMFLOAT2 radian = { XMConvertToRadians(cameraTarget.x + 90),XMConvertToRadians(cameraTarget.x) };

		//右入力
		if (input->PushKey(DIK_S)) {
			cameraPos.x += Pspeed * cos(radian.x);
			cameraPos.z += Pspeed * sin(radian.x);
		}
		//左入力
		if (input->PushKey(DIK_W)) {
			cameraPos.x -= Pspeed * cos(radian.x);
			cameraPos.z -= Pspeed * sin(radian.x);
		}
		//下入力
		if (input->PushKey(DIK_D)) {
			cameraPos.x -= Pspeed * cos(radian.y);
			cameraPos.z -= Pspeed * sin(radian.y);
		}
		//上入力
		if (input->PushKey(DIK_A)) {
			cameraPos.x += Pspeed * cos(radian.y);
			cameraPos.z += Pspeed * sin(radian.y);
		}
		//下入力
		if (input->PushKey(DIK_SPACE)) {
			cameraPos.y += Pspeed;
		}
		//上入力
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