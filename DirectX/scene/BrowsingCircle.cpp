#include "BrowsingCircle.h"
#include "SceneManager.h"
#include "TestField.h"
#include "DirectInput.h"
#include <imgui.h>

using namespace DirectX;

void BrowsingCircle::Initialize()
{
	//円モデル読み込み
	SpherePBRModel = FbxModel::Create("SpherePBR");

	int x = 0;
	int y = 0;

	//左上座標
	XMFLOAT3 leftUp = { -40,-40,0 };
	float wide = 20.0f;

	for (int i = 0; i < circle.size(); i++)
	{
		circle[i] = Fbx::Create(SpherePBRModel.get());
		circle[i]->SetPosition({ leftUp.x + x * wide,leftUp.y - y * wide,leftUp.z });
		circle[i]->SetScale({ 5,5,5 });
		circle[i]->SetMetalness((float)(1 / 25) * i);
		x++;
		if (x == 5)
		{
			x = 0;
			y--;
		}
	}
}

void BrowsingCircle::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	//カメラ初期化
	camera->SetEye({ 0,0,-90 });
	camera->SetTarget({ 0,0,0 });

	//シーンの移行
	if (input->TriggerKey(DIK_0))
	{
		TestField* nextScene = new TestField();
		nextScene->Initialize();
		SceneManager::SetNextScene(nextScene);
	}

	for (int i = 0; i < circle.size(); i++)
	{
		circle[i]->Update();
	}
}

void BrowsingCircle::Draw()
{
	Fbx::PreDraw(cmdList);
	for (int i = 0; i < circle.size(); i++)
	{
		circle[i]->Draw();
	}
	Fbx::PostDraw();
}

void BrowsingCircle::Finalize()
{
}

void BrowsingCircle::ImguiDraw()
{
	float baseColor[3];//ベースカラ―
	float metalness;//金属度(0 or 1)
	float specular;//鏡面反射度
	float roughness;//粗さ

	baseColor[0] = circle[0]->GetBaseColor().x;
	baseColor[1] = circle[0]->GetBaseColor().y;
	baseColor[2] = circle[0]->GetBaseColor().z;
	metalness = circle[0]->GetMetalness();
	specular = circle[0]->GetSpecular();
	roughness = circle[0]->GetRoughness();

	ImGui::Begin("Material");
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(300, 130));
	ImGui::ColorEdit3("baseColor", baseColor, ImGuiColorEditFlags_Float);
	ImGui::SliderFloat("metalness", &metalness, 0, 1);
	ImGui::SliderFloat("specular", &specular, 0, 1);
	ImGui::SliderFloat("roughness", &roughness, 0, 1);
	ImGui::End();

	circle[0]->SetBaseColor({ baseColor[0],baseColor[1],baseColor[2] });
	circle[0]->SetMetalness(metalness);
	circle[0]->SetSpecular(specular);
	circle[0]->SetRoughness(roughness);
}

void BrowsingCircle::GetConstbufferNum()
{
}
