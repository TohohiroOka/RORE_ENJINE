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

	//�n�`
	ground[0] = Ground::Create("heightmap01.bmp", "jimen.png", "kabe.png");

	//�e�}�l�[�W���[
	bullet = BulletManager::Create();

	//�G�}�l�[�W���[
	enemy = EnemyManager::Create();

	//�{�X
	boss = BossA::Create({ 3825.0f / 2.0f ,100.0f ,3825.0f / 2.0f });
}

void Boss1::Update()
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	player->Update(cameraAngle);

	XMFLOAT3 playerPos = player->GetPosition();

	//�e�X�V
	bullet->Update();
	if (bullet->CheckEnemyBCollision(playerPos))
	{
		player->Damage();
	}

	////�G�X�V
	//enemy->Update(playerPos);
	//enemy->CheckCollision(playerPos);

	//�G�ƒe�̓����蔻��
	auto& pBullet = bullet->GetPlayerBullet();
	//auto& enemyA = enemy->GetEnemyA();
	//for (auto& b : pBullet)
	//{
	//	for (auto& e : enemyA)
	//	{
	//		//������Ȃ�������X�L�b�v
	//		if (!GameCollision::CheckBulletToEnemy(b.get(), e.get())) {continue;}
	//		b->SetIsAlive(false);
	//		e->Damage();
	//	}
	//}

	//�n�`�X�V
	for (int i = 0; i < ground_num; i++)
	{
		ground[i]->Update();
	}

	//�{�X�X�V
	//boss->SetMove(playerPos);
	boss->Update();
	for (auto& b : pBullet)
	{
		//������Ȃ�������X�L�b�v
		if (!GameCollision::CheckBulletToEnemy(b.get(), boss.get())) { continue; }
		b->SetIsAlive(false);
		boss->Damage();
	}

	//�J�����X�V
	CameraUpdate();

	//�{�X���|���ꂽ�̂Ńt���A�I���
	if (!boss->GetIsAlive())
	{
		Title* nextScene = new Title();
		nextScene->Initialize();
		SceneManager::SetNextScene(nextScene);
	}

	//�v���C���[��hp��0�ɂȂ����̂ŏI��
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
	//float baseColor[3];//�x�[�X�J���\
	//float metalness;//�����x(0 or 1)
	//float specular;//���ʔ��˓x
	//float roughness;//�e��

	//baseColor[0] = anm->GetBaseColor().x;
	//baseColor[1] = anm->GetBaseColor().y;
	//baseColor[2] = anm->GetBaseColor().z;
	//metalness = anm->GetMetalness();
	//specular = anm->GetSpecular();
	//roughness = anm->GetRoughness();

	////���C�gon/off
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

	//�J�����X�V
	float cameraRadius = DirectX::XMConvertToRadians(cameraAngle);
	const float range = 100.0f;
	XMFLOAT3 playerPos = player->GetPosition();
	camera->SetEye({ cosf(cameraRadius) * range + playerPos.x,playerPos.y + cameraY,sinf(cameraRadius) * range + playerPos.z });
	camera->SetTarget(playerPos);
}
