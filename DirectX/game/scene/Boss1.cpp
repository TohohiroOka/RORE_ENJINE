#include "Boss1.h"
#include "SceneManager.h"
#include "Title.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"
#include "Easing.h"
#include "GameHelper.h"

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
	ground[0] = Ground::Create("heightmap01.bmp", "jimen.png", "kabe.png");

	//弾マネージャー
	bullet = BulletManager::Create();

	//敵マネージャー
	enemy = EnemyManager::Create();

	//ボス
	boss = BossA::Create({ 3825.0f / 2.0f ,100.0f ,3825.0f / 2.0f });

	//ターゲット
	isTarget = false;
	//ターゲット時のイージング
	isTageEase = false;
	//イージングタイマー
	easeTimer = 0;
	//イージング初期値点
	startAngle = 0;
	//イージング初期値点
	startTarget = { 0,0,0 };
	//イージング初期値点
	startPos = { 0,0,0 };
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

	//ボス更新
	//boss->SetMove(playerPos);
	boss->Update();

	//ボスとプレイヤー弾の判定
	if (bullet->CheckPlayerBulletToEnemyCollision(boss->GetPosition(),boss->GetScale())) {
		boss->Damage();
	}

	//地形更新
	for (int i = 0; i < ground_num; i++)
	{
		ground[i]->Update();
	}

	//カメラ更新
	CameraUpdate();

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

	InterfaceObject3d::SetCmdList(cmdList);

	HeightMap::PreDraw();
	for (int i = 0; i < ground_num; i++)
	{
		ground[i]->Draw();
	}

	Object3d::PreDraw();
	player->Draw();
	bullet->Draw();
	//enemy->Draw();
	boss->Draw();

	//PrimitiveObject3D::PreDraw();
	//ground[0]->CDraw();

	InterfaceObject3d::ReleaseCmdList();

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

	//プレイヤー座標
	XMFLOAT3 playerPos = player->GetPosition();
	const float range = 100.0f;

	//ターゲットon/off
	if (input->TriggerKey(DIK_P))
	{
		startTarget = camera->GetTarget();
		startPos = camera->GetEye();
		startAngle = cameraAngle;
		isTarget = (isTarget + 1) % 2;
		isTageEase = true;
		easeTimer = 0;
	}

	//カメラ更新
	XMFLOAT3 target = {};
	Vector3 eyeVec = {};
	XMFLOAT3 eye = {};
	
	{
		////ターゲット
		//if (isTarget)
		//{
		//	target = boss->GetPosition();
		//	eyeVec = { playerPos.x - target.x, playerPos.y - target.y , playerPos.z - target.z };
		//	eyeVec = eyeVec.normalize();
		//	eye = {
		//		playerPos.x + eyeVec.x * range,
		//		playerPos.y + eyeVec.y * range,
		//		playerPos.z + eyeVec.z * range };

		//	if (isTageEase)
		//	{
		//		easeTimer++;
		//		const float maxTime = 40.0f;
		//		float rate = easeTimer / maxTime;

		//		float angle = GetAngle(startTarget, boss->GetPosition());
		//		cameraAngle = Easing::OutQuart(startAngle, angle, rate);

		//		target.x = Easing::OutQuart(startTarget.x, target.x, rate);
		//		target.y = Easing::OutQuart(startTarget.y, target.y, rate);
		//		target.z = Easing::OutQuart(startTarget.z, target.z, rate);

		//		eye.x = Easing::OutQuart(startPos.x, eye.x, rate);
		//		eye.y = Easing::OutQuart(startPos.y, eye.y, rate);
		//		eye.z = Easing::OutQuart(startPos.z, eye.z, rate);

		//		if (rate >= 1.0f)
		//		{
		//			easeTimer = 0;
		//			isTageEase = false;
		//		}
		//	}
		//	else {
		//		cameraAngle = GetAngle(playerPos, boss->GetPosition());
		//	}
		//}
		////ターゲット無し
		//else
	}
	{
		float cameraRadius = DirectX::XMConvertToRadians(cameraAngle);
		target = playerPos;
		eye = {
			cosf(cameraRadius) * range + playerPos.x,
			playerPos.y + cameraY,
			sinf(cameraRadius) * range + playerPos.z };

		if (isTageEase)
		{
			easeTimer++;
			const float maxTime = 40.0f;
			float rate = easeTimer / maxTime;

			target.x = Easing::OutQuart(startTarget.x, target.x, rate);
			target.y = Easing::OutQuart(startTarget.y, target.y, rate);
			target.z = Easing::OutQuart(startTarget.z, target.z, rate);

			eye.x = Easing::OutQuart(startPos.x, eye.x, rate);
			eye.y = Easing::OutQuart(startPos.y, eye.y, rate);
			eye.z = Easing::OutQuart(startPos.z, eye.z, rate);

			if (rate >= 1.0f)
			{
				easeTimer = 0;
				isTageEase = false;
			}
		}
		else
		{
			if (input->PushKey(DIK_LEFT)) { cameraAngle += 3.0f; }
			if (input->PushKey(DIK_RIGHT)) { cameraAngle -= 3.0f; }
		}
	}

	cameraAngle = float(int(cameraAngle) % 360);
	camera->SetTarget(target);
	camera->SetEye(eye);

	DebugText* text = DebugText::GetInstance();
	std::string strCameraA = std::to_string(cameraAngle);
	std::string strEaseTimer = std::to_string(easeTimer);
	std::string eyeX = std::to_string(eye.x);
	std::string eyeY = std::to_string(eye.y);
	std::string eyeZ = std::to_string(eye.z);
	text->Print("cameraAngle : " + strCameraA + "easeTimer : " + strEaseTimer, 100, 250);
	text->Print("eye :: x : " + eyeX + "y : " + eyeY + "z : " + eyeZ, 100, 280);
	text = nullptr;
}
