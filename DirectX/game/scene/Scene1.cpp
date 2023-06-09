#include "Scene1.h"
#include "SceneManager.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"
#include "WindowApp.h"
#include <imgui.h>
#include "Collision.h"

using namespace DirectX;

char Scene1::fileName[36] = "";

const std::array<XMFLOAT4, 2> COLOR = { XMFLOAT4{ 0.0f,0.0f,0.8f,1.0f } ,{ 0.8f,0.0f,0.0f,1.0f } };

void Scene1::Initialize()
{
	camera = nullptr;

	map = Map::Create();
	isDrawLine = true;
	isSetObject = { false,false,true };

	cameraPos = { 0,0,-10 };
	cameraTarget = { 135,0,0 };

	imguiPos = { 0,0 };
	kaburi = false;

	exportTimer = 100;
	improtTimer = 100;
	isImprot = false;

	imguiColor = COLOR[0];

	isOutsideCollision = true;

	mapChangeDirection = true;

	model = Model::CreateFromOBJ("NormalCube");
	obj = Object3d::Create(model.get());
	obj->SetScale({ 10.0f,10.0f ,10.0f });

	Sprite::LoadTexture("amm","Resources/amm.jpg");
	sprite = Sprite::Create("amm");
	sprite->Update();
}

void Scene1::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	obj->Update();

	//フレームの最初に初期化するもの
	kaburi = false;
	if (mapChange) {
		map->ChangeDelimitNum(mapChangeDirection, mapChangeSize);
		mapChange = false;
	}
	if (isImprot) {
		if (!map->ImputMap(fileName)) {
			imguiColor = COLOR[1];
		}
		improtTimer = 0;
		isImprot = false;
	}
	//マウス座標
	XMFLOAT2 mousePos = input->GetMousePoint();
	Vector3 pout = {};
	Vector3 target = {};
	Collision::CalcScreenToWorld(&pout, camera, mousePos, 0.0f);
	Collision::CalcScreenToWorld(&target, camera, mousePos, 1.0f);

	if (!kaburi) {
		//オブジェクト設置
		if (input->TriggerMouseButton(DirectInput::MOUSE_BUTTON::MOUSE_LEFT)) {
			map->AddBox(camera->GetEye(), isSetObject);
		} else if (input->TriggerMouseButton(DirectInput::MOUSE_BUTTON::MOUSE_RIGHT)) {
			map->DeleteBox(camera->GetEye());
		}
	}

	if (input->PushKey(DIK_LCONTROL) && input->TriggerKey(DIK_Z)) {
		map->Undo();
	} else if (input->PushKey(DIK_LCONTROL) && input->TriggerKey(DIK_Y)) {
		map->Redo();
	}

	if (input->TriggerKey(DIK_1)) {
		for (int i = 0; i < 9; i++) {
			if (i == 0) { isSetObject[i] = true; } else { isSetObject[i] = false; }
		}
	}
	if (input->TriggerKey(DIK_2)) {
		for (int i = 0; i < 9; i++) {
			if (i == 1) { isSetObject[i] = true; } else { isSetObject[i] = false; }
		}
	}
	if (input->TriggerKey(DIK_3)) {
		for (int i = 0; i < 9; i++) {
			if (i == 2) { isSetObject[i] = true; } else { isSetObject[i] = false; }
		}
	}
	if (input->TriggerKey(DIK_4)) {
		isDrawLine = !isDrawLine;
	}
	if (input->TriggerKey(DIK_5)) {
		isOutsideCollision = !isOutsideCollision;
	}
	if (input->TriggerKey(DIK_F1)) {
		for (int i = 0; i < 9; i++) {
			if (i == 3) { isSetObject[i] = true; } else { isSetObject[i] = false; }
		}
	}
	if (input->TriggerKey(DIK_F2)) {
		for (int i = 0; i < 9; i++) {
			if (i == 4) { isSetObject[i] = true; } else { isSetObject[i] = false; }
		}
	}
	if (input->TriggerKey(DIK_F3)) {
		for (int i = 0; i < 9; i++) {
			if (i == 5) { isSetObject[i] = true; } else { isSetObject[i] = false; }
		}
	}
	if (input->TriggerKey(DIK_F4)) {
		for (int i = 0; i < 9; i++) {
			if (i == 6) { isSetObject[i] = true; } else { isSetObject[i] = false; }
		}
	}
	if (input->TriggerKey(DIK_F5)) {
		for (int i = 0; i < 9; i++) {
			if (i == 7) { isSetObject[i] = true; } else { isSetObject[i] = false; }
		}
	}
	if (input->TriggerKey(DIK_F6)) {
		for (int i = 0; i < 9; i++) {
			if (i == 8) { isSetObject[i] = true; } else { isSetObject[i] = false; }
		}
	}

	map->Update(pout, target, kaburi, isOutsideCollision);

	DebugText* text = DebugText::GetInstance();
	if (exportTimer != 100) {
		exportTimer++;
		text->Print("export", 100, 100, { 1.0f,1.0f ,1.0f }, 20);
		if (exportTimer == 20) { exportTimer = 100; }
	}
	if (improtTimer != 100) {
		improtTimer++;
		if (improtTimer == 40) {
			exportTimer = 100;
			imguiColor = COLOR[0];
		}
	}
	text = nullptr;
}

void Scene1::Draw(const int _cameraNum)
{
	map->SetLight(_cameraNum == 0);

	obj->Draw();

	//line
	if (isDrawLine) {
		map->Draw();
	}

	//Instance
	map->InstanceDraw();

	//スプライト
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
	map->FrameReset();
}