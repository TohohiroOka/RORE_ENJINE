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

	//�n�`
	ground[0] = Ground::Create("heightmap01.bmp", "jimen.png", "kabe.png");

	//�e�}�l�[�W���[
	bullet = BulletManager::Create();

	//�G�}�l�[�W���[
	enemy = EnemyManager::Create();

	//�{�X
	boss = BossA::Create({ 3825.0f / 2.0f ,100.0f ,3825.0f / 2.0f });

	//�^�[�Q�b�g
	isTarget = false;
	//�^�[�Q�b�g���̃C�[�W���O
	isTageEase = false;
	//�C�[�W���O�^�C�}�[
	easeTimer = 0;
	//�C�[�W���O�����l�_
	startAngle = 0;
	//�C�[�W���O�����l�_
	startTarget = { 0,0,0 };
	//�C�[�W���O�����l�_
	startPos = { 0,0,0 };
}

void Boss1::Update()
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//------�I�u�W�F�N�g�̍X�V--------//

	player->Update(cameraAngle);

	XMFLOAT3 playerPos = player->GetPosition();

	//�e�X�V
	bullet->Update(playerPos);
	if (bullet->CheckEnemyBulletToPlayerCollision())
	{
		player->Damage();
	}

	//�{�X�X�V
	//boss->SetMove(playerPos);
	boss->Update();

	//�{�X�ƃv���C���[�e�̔���
	if (bullet->CheckPlayerBulletToEnemyCollision(boss->GetPosition(),boss->GetScale())) {
		boss->Damage();
	}

	//�n�`�X�V
	for (int i = 0; i < ground_num; i++)
	{
		ground[i]->Update();
	}

	//�J�����X�V
	CameraUpdate();

	//------�X�V�ȊO�̏���--------//

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

	//�v���C���[���W
	XMFLOAT3 playerPos = player->GetPosition();
	const float range = 100.0f;

	//�^�[�Q�b�gon/off
	if (input->TriggerKey(DIK_P))
	{
		startTarget = camera->GetTarget();
		startPos = camera->GetEye();
		startAngle = cameraAngle;
		isTarget = (isTarget + 1) % 2;
		isTageEase = true;
		easeTimer = 0;
	}

	//�J�����X�V
	XMFLOAT3 target = {};
	Vector3 eyeVec = {};
	XMFLOAT3 eye = {};
	
	{
		////�^�[�Q�b�g
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
		////�^�[�Q�b�g����
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
