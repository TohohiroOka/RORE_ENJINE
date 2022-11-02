#include "Boss1.h"
#include "SceneManager.h"
#include "Title.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"
#include "Easing.h"

#include <imgui.h>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <memory>

#include "GameCollision.h"
#include "GameHelper.h"

using namespace DirectX;

void Boss1::Initialize()
{
	//プレイヤー
	player = Player::Create({ mapSize / 2.0f ,250.0f,mapSize / 5.0f });

	//地形
	ground = Ground::Create("heightmap01.bmp", "jimen.png", "kabe.png");

	//弾マネージャー
	bullet = BulletManager::Create();

	//敵マネージャー
	enemy = EnemyManager::Create();

	//ボス
	boss = BossA::Create({ mapSize / 2.0f ,100.0f ,mapSize / 2.0f });

	//UI
	ui = UiManager::Create(boss->GetMaxHp());
}

void Boss1::Update()
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//------オブジェクトの更新--------//

	player->Update(cameraAngle);

	XMFLOAT3 playerPos = player->GetPosition();

	//弾更新
	bullet->Update(playerPos);
	if (bullet->CheckEnemyBulletToPlayerCollision())
	{
		player->Damage();
	}

	//敵の更新
	enemy->Update(playerPos);
	enemy->CheckCollision();

	//ボス更新
	//boss->SetMove(playerPos);
	boss->Update();

	//ボスとプレイヤー弾の判定
	if (bullet->CheckPlayerBulletToEnemyCollision(boss->GetPosition(),boss->GetScale())) {
		boss->Damage();
	}

	//ui更新
	ui->Update(boss->GetHp());

	//------更新以外の処理--------//

	//ボスが倒されたのでフロア終わり
	if (!boss->GetIsAlive())
	{
		Title* nextScene = new Title();
		nextScene->Initialize();
		SceneManager::SetNextScene(nextScene);
	}

	//プレイヤーのhpが0になったので終了
	if (!player->GetIsAlive())
	{
		Title* nextScene = new Title();
		nextScene->Initialize();
		SceneManager::SetNextScene(nextScene);
	}

	input = nullptr;
	xinput = nullptr;
}

void Boss1::Draw()
{
	assert(cmdList);

	XMFLOAT3 position = player->GetPosition();
	std::string strX = std::to_string(position.x);
	std::string strY = std::to_string(position.y);
	std::string strZ = std::to_string(position.z);
	DebugText::GetInstance()->Print("Upos :: x : " + strX + "y : " + strY + "z : " + strZ, 100, 125);


	Sprite::PreDraw(cmdList);
	ui->Draw();
	DebugText::GetInstance()->DrawAll();
	Sprite::PostDraw();

	InterfaceObject3d::SetCmdList(cmdList);

	HeightMap::PreDraw();
	ground->Draw();

	Object3d::PreDraw();
	ground->CubeDraw();
	player->Draw();
	enemy->Draw();
	boss->Draw();

	//PrimitiveObject3D::PreDraw();
	//ground[0]->CDraw();

	InterfaceObject3d::ReleaseCmdList();
	
	InstanceObject::PreDraw(cmdList);
	bullet->Draw();
	InstanceObject::PostDraw();
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

void Boss1::CameraUpdate(Camera* camera)
{
	DirectInput* input = DirectInput::GetInstance();

	if (input->PushKey(DIK_LEFT)) { cameraAngle += 3.0f; }
	if (input->PushKey(DIK_RIGHT)) { cameraAngle -= 3.0f; }

	//プレイヤー座標
	XMFLOAT3 playerPos = player->GetPosition();
	const float range = 15.0f;
	float cameraRadius = DirectX::XMConvertToRadians(cameraAngle);
	XMFLOAT3 target = playerPos;
	XMFLOAT3 eye = {
		cosf(cameraRadius) * range + playerPos.x,
		playerPos.y + cameraY,
		sinf(cameraRadius) * range + playerPos.z };

	cameraAngle = float(int(cameraAngle) % 360);
	camera->SetTarget(target);
	camera->SetEye(eye);

	//カメラの傾き
	XMFLOAT3 playerTilt = player->GetObjAngle();
	camera->SetUp({ playerTilt.z,1,0 });

	DebugText* text = DebugText::GetInstance();
	std::string strCameraA = std::to_string(cameraAngle);
	std::string eyeX = std::to_string(eye.x);
	std::string eyeY = std::to_string(eye.y);
	std::string eyeZ = std::to_string(eye.z);
	text->Print("eye :: x : " + eyeX + "y : " + eyeY + "z : " + eyeZ, 100, 300);
	text = nullptr;
}
