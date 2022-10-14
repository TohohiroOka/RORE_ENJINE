#include "Boss1.h"
#include "SceneManager.h"
#include "Title.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"

#include <imgui.h>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <memory>

#include "GameCollision.h"

using namespace DirectX;

void Boss1::Initialize()
{
	player = Player::Create();

	//地形
	ground[0] = Ground::Create("heightmap03.bmp", "jimen.png", "kabe.png");

	bullet = BulletManager::Create();
	FixedTurret::StaticInitialize();
	fixedTurret[0] = FixedTurret::Create({ 1905,50,3000 }, { 0.0f,0.2f,-1.0f });//-z座標向き
	fixedTurret[1] = FixedTurret::Create({ 1905,50,810 }, { 0.0f,0.2f,1.0f });//z座標向き
	fixedTurret[2] = FixedTurret::Create({ 3000,50,1905 }, { -1.0f,0.2f,0.0f });//-x座標向き
	fixedTurret[3] = FixedTurret::Create({ 810,50,1905 }, { 1.0f,0.2f,0.0f });//x座標向き

	enemy = EnemyManager::Create();
}

void Boss1::Update()
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//シーンの移行
	if (input->TriggerKey(DIK_0))
	{
		Title* nextScene = new Title();
		nextScene->Initialize();
		SceneManager::SetNextScene(nextScene);
	}

	player->SetCameraAngle(cameraAngle);
	player->Update();

	bullet->Update();
	bullet->CheckEnemyBCollision(player->GetPosition());

	enemy->Update(player->GetPosition());
	enemy->CheckCollision(player->GetPosition());

	auto& pBullet = bullet->GetPlayerBullet();
	auto& enemyA = enemy->GetEnemyA();
	for (auto& b : pBullet)
	{
		for (auto& e : enemyA)
		{
			//当たらなかったらスキップ
			if (!GameCollision::CheckBulletToEnemy(b.get(), e.get())) {continue;}
			b->SetIsAlive(false);
			e->Damage();
		}
	}

	for (auto& i : fixedTurret)
	{
		i->Update();
	}

	for (int i = 0; i < ground_num; i++)
	{
		ground[i]->Update();
	}

	CameraUpdate();

	input = nullptr;
	xinput = nullptr;
}

void Boss1::Draw()
{
	assert(cmdList);

	InterfaceObject3d::SetCmdList(cmdList);

	//HeightMap::PreDraw();
	//for (int i = 0; i < ground_num; i++)
	//{
	//	ground[i]->Draw();
	//}

	Object3d::PreDraw();
	player->Draw();
	bullet->Draw();
	for (auto& i : fixedTurret)
	{
		i->Draw();
	}
	enemy->Draw();

	PrimitiveObject3D::PreDraw();
	ground[0]->CDraw();

	InterfaceObject3d::ReleaseCmdList();

	//DrawLine3D::PreDraw(cmdList);
	//bullet->DLDraw();
	//DrawLine3D::PostDraw();

	Sprite::PreDraw(cmdList);
	DebugText::GetInstance()->DrawAll();
	Sprite::PostDraw();
}

void Boss1::Finalize()
{
}

void Boss1::ImguiDraw()
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

void Boss1::CameraUpdate()
{
	DirectInput* input = DirectInput::GetInstance();

	if (input->PushKey(DIK_LEFT)) { cameraAngle += 3.0f; }
	if (input->PushKey(DIK_RIGHT)) { cameraAngle -= 3.0f; }

	//カメラ更新
	float cameraRadius = DirectX::XMConvertToRadians(cameraAngle);
	const float range = 100.0f;
	XMFLOAT3 playerPos = player->GetPosition();
	camera->SetEye({ cosf(cameraRadius) * range + playerPos.x,playerPos.y + cameraY,sinf(cameraRadius) * range + playerPos.z });
	camera->SetTarget(playerPos);
}
