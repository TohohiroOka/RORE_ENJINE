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
#include "JsonLoder.h"

using namespace DirectX;

void Boss1::Initialize()
{
	//�V�[��
	scene = SCENE::SET;
	//�J�����̉�]
	cameraAngle = -90;
	//�^�C�}�[
	timer = 0;
	//�J�����̈�ԉ�
	cameraBack = 4000.0f;

	//Json�t�@�C���ǂݍ��݂Ŏg�p����I�u�W�F�N�g���z��
	std::vector<std::string> objectName = {
		"player",
	};

	JsonObjectData* jData = JsonLoder::LoadFile("map2.json", objectName);
	JsonMoveData* jBossMoveData = JsonLoder::LoadMoveFile("map2_boss_move.json");

	//�n�`
	ground = Ground::Create("heightmap01.bmp", "jimen.png", "kabe.png");

	//�v���C���[
	player = Player::Create(jData->objects[objectName[0]][0].pos);

	//�e�}�l�[�W���[
	bullet = BulletManager::Create();

	//�r�[���}�l�[�W���[
	beam = BeamManager::Create();

	//�G�}�l�[�W���[
	enemy = EnemyManager::Create();

	//�{�X
	for (auto& i : jBossMoveData->movePoint) {
		boss->SetMoveList(i.pos, i.moveList);
	}
	boss = BossA::Create(jBossMoveData->movePoint[0].pos, 0);

	//UI
	ui = UiManager::Create(player->GetMaxHp(),boss->GetMaxHp());
}

void Boss1::Update()
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//------�I�u�W�F�N�g�̍X�V--------//
	XMFLOAT3 playerPos = player->GetPosition();

	if (scene == SCENE::SET)
	{
		timer++;
		const float maxTime = 1.0f;
		float ratio = float(timer) / maxTime;
		ground->SetRatio(ratio);
		if (ratio >= 1.0f) {
			ground->SetRatio(1.0f);
			scene = SCENE::SET_FROM_PLAY;
			timer = 0;
		}
	}
	else if (scene == SCENE::SET_FROM_PLAY)
	{
		timer++;
		const float maxTime = 100.0f;
		if (timer >= maxTime) {
			timer = 0;
			scene = SCENE::PLAY;
		}
	}
	else if (scene == SCENE::PLAY)
	{
		//�v���C���[�X�V
		player->Update(cameraAngle);

		//�e�X�V
		bullet->Update(playerPos);
		if (bullet->CheckEnemyBulletToPlayerCollision())
		{
			player->Damage();
		}

		//�r�[���̍X�V
		beam->Update();

		//�G�̍X�V
		enemy->Update(playerPos);
		enemy->CheckCollision();

		//�{�X�X�V
		//boss->SetMove(playerPos);
		boss->Update();

		//�{�X�ƃv���C���[�e�̔���
		if (bullet->CheckPlayerBulletToEnemyCollision(boss->GetPosition(), boss->GetScale())) {
			boss->Damage();
		}

		//ui�X�V
		ui->Update(player->GetHp(), boss->GetHp());

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
	}

	input = nullptr;
	xinput = nullptr;
}

void Boss1::DrawNotPostB()
{
	InstanceObject::PreDraw(cmdList);
	bullet->Draw();
	beam->Draw();
	InstanceObject::PostDraw();
}

void Boss1::Draw()
{
	assert(cmdList);

	InterfaceObject3d::SetCmdList(cmdList);

	HeightMap::PreDraw();
	ground->Draw();

	Object3d::PreDraw();
	ground->CubeDraw();
	player->Draw();
	enemy->Draw();
	boss->Draw();

	//if (scene == SCENE::SET)
	//{
	//	PrimitiveObject3D::PreDraw();
	//	ground->CDraw();
	//}

	InterfaceObject3d::ReleaseCmdList();
}

void Boss1::DrawNotPostA()
{
	Sprite::PreDraw(cmdList);
	ui->Draw();
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

void Boss1::CameraUpdate(Camera* camera)
{
	DirectInput* input = DirectInput::GetInstance();

	XMFLOAT3 target = {};
	XMFLOAT3 eye = {};

	if (scene == SCENE::SET)
	{
		//����̃J����
		const XMFLOAT3 initTarget = { mapSize / 2.0f,200.0f,mapSize / 2.0f };
		const XMFLOAT3 initEye = { mapSize / 2.0f,900.0f,mapSize / 10.0f };
		camera->SetTarget(initTarget);
		camera->SetEye(initEye);
		camera->SetMatProjection(cameraBack);
	}
	else if (scene == SCENE::SET_FROM_PLAY)
	{
		const float maxTime = 100.0f;
		float ratio = float(timer) / maxTime;

		const XMFLOAT3 initTarget = { mapSize / 2.0f,200.0f,mapSize / 2.0f };
		const XMFLOAT3 initEye = { mapSize / 2.0f,900.0f,mapSize / 10.0f };
		XMFLOAT3 playerPos = player->GetPosition();

		target.x = Easing::OutCubic(initTarget.x, playerPos.x, ratio);
		target.y = Easing::OutCubic(initTarget.y, playerPos.y, ratio);
		target.z = Easing::OutCubic(initTarget.z, playerPos.z, ratio);

		const float range = 20.0f;
		float cameraRadius = DirectX::XMConvertToRadians(cameraAngle);
		XMFLOAT3 afterEye = {
			cosf(cameraRadius) * range + playerPos.x,
			playerPos.y + cameraY,
			sinf(cameraRadius) * range + playerPos.z };

		eye.x = Easing::OutCubic(initEye.x, afterEye.x, ratio);
		eye.y = Easing::OutCubic(initEye.y, afterEye.y, ratio);
		eye.z = Easing::OutCubic(initEye.z, afterEye.z, ratio);

		float afterCameraBack = 3000.0f;
		float initCameraBack = 4000.0f;
		cameraBack= Easing::OutCubic(initCameraBack, afterCameraBack, ratio);

		camera->SetTarget(target);
		camera->SetEye(eye);
		camera->SetMatProjection(cameraBack);
	}
	else if (scene == SCENE::PLAY)
	{
		if (input->PushKey(DIK_LEFT)) { cameraAngle += 3.0f; }
		if (input->PushKey(DIK_RIGHT)) { cameraAngle -= 3.0f; }

		//�v���C���[���W
		XMFLOAT3 playerPos = player->GetPosition();
		const float range = 20.0f;
		float cameraRadius = DirectX::XMConvertToRadians(cameraAngle);
		target = playerPos;
		eye = {
			cosf(cameraRadius) * range + playerPos.x,
			playerPos.y + cameraY,
			sinf(cameraRadius) * range + playerPos.z };

		cameraAngle = float(int(cameraAngle) % 360);
		camera->SetTarget(target);
		camera->SetEye(eye);
	}

	DebugText* text = DebugText::GetInstance();
	std::string strCameraA = std::to_string(cameraAngle);
	std::string eyeX = std::to_string(eye.x);
	std::string eyeY = std::to_string(eye.y);
	std::string eyeZ = std::to_string(eye.z);
	text->Print("eye :: x : " + eyeX + "y : " + eyeY + "z : " + eyeZ, 100, 300);
	text = nullptr;
}
