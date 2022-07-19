#include "BrowsingCircle.h"
#include "SceneManager.h"
#include "TestField.h"
#include "DirectInput.h"
#include <imgui.h>
#include "Easing.h"
#include "WindowApp.h"

using namespace DirectX;

void BrowsingCircle::Initialize()
{
	////円モデル読み込み
	//SpherePBRModel = FbxModel::Create("SpherePBR");

	//circle = Fbx::Create(SpherePBRModel.get());
	//circle->SetPosition({ 0,0,0 });
	//circle->SetScale({ 5,5,5 });
}

void BrowsingCircle::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	if (input->PushKey(DIK_LEFT))
	{
		cameraAngle++;
	}
	else if (input->PushKey(DIK_RIGHT))
	{
		cameraAngle--;
	}

	float UP_DOWN_MAX = 50.0f;
	if (input->PushKey(DIK_UP))
	{
		cameraY++;
		if (cameraY > UP_DOWN_MAX)
		{
			cameraY = UP_DOWN_MAX;
		}
	}
	else if (input->PushKey(DIK_DOWN))
	{
		cameraY--;
		if (cameraY < -UP_DOWN_MAX)
		{
			cameraY = -UP_DOWN_MAX;
		}
	}

	camera->SetTarget({ 0,0,0 });

	float radian = XMConvertToRadians(cameraAngle);
	XMFLOAT3 eye = {};
	eye.x = cosf(radian) * 30.0f;
	eye.y = cameraY;
	eye.z = sinf(radian) * 30.0f;

	//カメラ初期化
	camera->SetEye(eye);

	//シーンの移行
	if (input->TriggerKey(DIK_0))
	{
		TestField* nextScene = new TestField();
		nextScene->Initialize();
		SceneManager::SetNextScene(nextScene);
	}

	//circle->Update();
}

void BrowsingCircle::Draw()
{
	//Fbx::PreDraw(cmdList);
	//circle->Draw();
	//Fbx::PostDraw();
}

void BrowsingCircle::Finalize()
{
}

void BrowsingCircle::ImguiDraw()
{
	//float baseColor[3];//ベースカラ―
	//float metalness;//金属度(0 or 1)
	//float specular;//鏡面反射度
	//float roughness;//粗さ

	//baseColor[0] = circle->GetBaseColor().x;
	//baseColor[1] = circle->GetBaseColor().y;
	//baseColor[2] = circle->GetBaseColor().z;
	//metalness = circle->GetMetalness();
	//specular = circle->GetSpecular();
	//roughness = circle->GetRoughness();

	////ライトon/off
	//static bool lightAct1 = false;
	//static bool lightAct2 = false;
	//static bool lightAct3 = false;

	//ImGui::Begin("Material");
	//ImGui::SetWindowPos(ImVec2(0, 0));
	//ImGui::SetWindowSize(ImVec2(300, 200));
	//ImGui::ColorEdit3("baseColor", baseColor, ImGuiColorEditFlags_Float);
	//ImGui::SliderFloat("metalness", &metalness, 0, 1);
	//ImGui::SliderFloat("specular", &specular, 0, 1);
	//ImGui::SliderFloat("roughness", &roughness, 0, 1);
	//ImGui::Checkbox("Light1", &lightAct1);
	//ImGui::Checkbox("Light2", &lightAct2);
	//ImGui::Checkbox("Light3", &lightAct3);
	//ImGui::End();

	//circle->SetBaseColor({ baseColor[0],baseColor[1],baseColor[2] });
	//circle->SetMetalness(metalness);
	//circle->SetSpecular(specular);
	//circle->SetRoughness(roughness);

	//light->SetDirLightActive(0, lightAct1);
	//light->SetDirLightActive(1, lightAct2);
	//light->SetDirLightActive(2, lightAct3);
}

void BrowsingCircle::GetConstbufferNum()
{
}
