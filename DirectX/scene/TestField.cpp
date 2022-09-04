#include "TestField.h"
#include "SceneManager.h"
#include "BrowsingCircle.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"

#include <imgui.h>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <memory>

using namespace DirectX;

void TestField::Initialize()
{
	player = Player::Create();

	//地形
	ground[0] = Ground::Create("heightmap01.bmp", "Dirt.jpg", "Grass.jpg");
	
	//ground[1] = Ground::Create("heightmap02.bmp", "Dirt.jpg", "Grass.jpg");
	//ground[1]->SetPos({ -2550.0f,0.0f,0.0f });
	
	//ground[2] = Ground::Create("heightmap03.bmp", "Dirt.jpg", "Grass.jpg");
	//ground[2]->SetPos({ 0.0f,0.0f,-2550.0f });
	//
	//ground[3] = Ground::Create("heightmap04.bmp", "Dirt.jpg", "Grass.jpg");
	//ground[3]->SetPos({ -2550.0f,0.0f,-2550.0f });

	enemy = EnemyManager::Create();
}

void TestField::Update()
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//シーンの移行
	if (input->TriggerKey(DIK_0))
	{
		BrowsingCircle* nextScene = new BrowsingCircle();
		nextScene->Initialize();
		SceneManager::SetNextScene(nextScene);
	}

	player->SetCameraAngle(cameraAngle);
	player->Update();
	for (int i = 0; i < ground_num; i++)
	{
		ground[i]->Update();
	}

	enemy->Update();

	CameraUpdate();

	if (input->TriggerKey(DIK_P))
	{
		player->BulletUpdate(camera->GetTarget());
	}

	input = nullptr;
	xinput = nullptr;
}

void TestField::Draw()
{
	assert(cmdList);

	InterfaceObject3d::SetCmdList(cmdList);

	HeightMap::PreDraw();
	for (int i = 0; i < ground_num; i++)
	{
		ground[i]->Draw();
	}

	Object3d::PreDraw();
	player->Draw();		
	enemy->Draw();

	InterfaceObject3d::ReleaseCmdList();
}

void TestField::Finalize()
{
}

void TestField::ImguiDraw()
{
	//float baseColor[3];//ベースカラ―
	//float metalness;//金属度(0 or 1)
	//float specular;//鏡面反射度
	//float roughness;//粗さ

	//baseColor[0] = anm->GetBaseColor().x;
	//baseColor[1] = anm->GetBaseColor().y;
	//baseColor[2] = anm->GetBaseColor().z;
	//metalness = anm->GetMetalness();
	//specular = anm->GetSpecular();
	//roughness = anm->GetRoughness();

	////ライトon/off
	//static bool lightAct1 = false;
	//static bool lightAct2 = false;
	//static bool lightAct3 = false;

	//ImGui::Begin("Material");
	//ImGui::SetWindowPos(ImVec2(0, 0));
	//ImGui::SetWindowSize(ImVec2(300, 130));
	//ImGui::ColorEdit3("baseColor", baseColor, ImGuiColorEditFlags_Float);
	//ImGui::SliderFloat("metalness", &metalness, 0, 1);
	//ImGui::SliderFloat("specular", &specular, 0, 1);
	//ImGui::SliderFloat("roughness", &roughness, 0, 1);
	//ImGui::Checkbox("Light1", &lightAct1);
	//ImGui::Checkbox("Light2", &lightAct2);
	//ImGui::Checkbox("Light3", &lightAct3);
	//ImGui::End();

	//anm->SetBaseColor({ baseColor[0],baseColor[1],baseColor[2] });
	//anm->SetMetalness(metalness);
	//anm->SetSpecular(specular);
	//anm->SetRoughness(roughness);

	//light->SetDirLightActive(0, lightAct1);
	//light->SetDirLightActive(1, lightAct2);
	//light->SetDirLightActive(2, lightAct3);
}

void TestField::CameraUpdate()
{
	DirectInput* input = DirectInput::GetInstance();

	if (input->TriggerKey(DIK_RETURN))
	{
		enemy->SetTarget();
	}

	if (!player->GetIsTargeting())
	{
		if (input->PushKey(DIK_LEFT)) { cameraAngle += 3.0f; }
		if (input->PushKey(DIK_RIGHT)) { cameraAngle -= 3.0f; }

		//カメラ更新
		float cameraEyeRadius = DirectX::XMConvertToRadians(cameraAngle);
		float cameraTargetRadius = DirectX::XMConvertToRadians(cameraAngle - 180);
		const float range = 100.0f;
		XMFLOAT3 playerPos = player->GetPos();
		camera->SetEye({ cosf(cameraEyeRadius) * range + playerPos.x,playerPos.y + cameraY,sinf(cameraEyeRadius) * range + playerPos.z });
		camera->SetTarget({ cosf(cameraTargetRadius) * range + playerPos.x,playerPos.y,sinf(cameraTargetRadius) * range + playerPos.z });
	}
	else
	{
		XMFLOAT2 vecPt = {};
		XMFLOAT3 playerPos = player->GetPos();
		XMFLOAT3 targetPos = player->GetTarget();
		vecPt.x = targetPos.x - playerPos.x;
		vecPt.y = targetPos.z - playerPos.z;

		//正規化
		float x = powf(targetPos.x - playerPos.x, 2);
		float y = powf(targetPos.z - playerPos.z, 2);
		float dist = sqrt(x + y);
		dist = 1.0f / dist;
		vecPt.x *= dist;
		vecPt.y *= dist;
		vecPt.x *= 50.0f;
		vecPt.y *= 50.0f;

		camera->SetEye({ playerPos.x - vecPt.x ,playerPos.y + cameraY/2,playerPos.z - vecPt.y });
		camera->SetTarget(player->GetTarget());
	}

	input = nullptr;
}
