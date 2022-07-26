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

void TestField::Initialize()
{
	playerModel = Model::CreateFromOBJ("uma");	
	groundModel = Model::CreateFromOBJ("wall");

	player = Player::Create(playerModel.get());
	ground = Ground::Create("heightmap01.bmp", "Dirt.jpg");

	heightmap = TouchableObject::Create(groundModel.get());
	heightmap->SetScale(100);
}

void TestField::Update()
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//�V�[���̈ڍs
	if (input->TriggerKey(DIK_0))
	{
		BrowsingCircle* nextScene = new BrowsingCircle();
		nextScene->Initialize();
		SceneManager::SetNextScene(nextScene);
	}

	if (input->PushKey(DIK_LEFT)) { cameraAngle+=3.0f; }
	if (input->PushKey(DIK_RIGHT)) { cameraAngle-= 3.0f; }
	if (input->PushKey(DIK_UP)) { cameraY+= 3.0f; }
	if (input->PushKey(DIK_DOWN)) { cameraY-= 3.0f; }

	//���W�A���ϊ�
	float radiusLR = DirectX::XMConvertToRadians(cameraAngle + 90.0f);
	float radiusUD = DirectX::XMConvertToRadians(cameraAngle);

	//player�ړ�
	float Pspeed = 5.0f;

	//�E����
	if (input->PushKey(DIK_A)) {
		cameraPos.x += Pspeed * cosf(radiusLR);
		cameraPos.z += Pspeed * sinf(radiusLR);
	}
	//������
	if (input->PushKey(DIK_D)) {
		cameraPos.x -= Pspeed * cosf(radiusLR);
		cameraPos.z -= Pspeed * sinf(radiusLR);
	}
	if (input->PushKey(DIK_W)) {
		cameraPos.x += Pspeed * cosf(radiusUD);
		cameraPos.z += Pspeed * sinf(radiusUD);
	}
	//������
	if (input->PushKey(DIK_S)) {
		cameraPos.x -= Pspeed * cosf(radiusUD);
		cameraPos.z -= Pspeed * sinf(radiusUD);
	}

	if (input->PushKey(DIK_Z)) {
		cameraPos.y += 3.0f;
	}
	//������
	if (input->PushKey(DIK_X)) {
		cameraPos.y -= 3.0f;
	}

	heightmap->Update();
	player->Update();
	ground->Update();

	//�J�����X�V
	float cameraRadius = DirectX::XMConvertToRadians(cameraAngle);
	const float range = 100.0f;
	XMFLOAT3 playerPos= player->GetPos();
	XMFLOAT3 cameraPos = playerPos;
	cameraPos.y += 40.0f;
	if (input->PushKey(DIK_Z))
	{
		cameraPos.z -= 100;
	}
	camera->SetEye({ cosf(cameraRadius) * range + cameraPos.x,cameraPos.y + cameraY,sinf(cameraRadius) * range + cameraPos.z });
	camera->SetTarget(playerPos);

	input = nullptr;
	xinput = nullptr;
}

void TestField::Draw()
{
	assert(cmdList);

	InterfaceObject3d::SetCmdList(cmdList);
	Object3d::PreDraw();
	//player->Draw();
	//heightmap->Draw();
	HeightMap::PreDraw();
	//ground->Draw();

	PrimitiveObject3D::PreDraw();
	ground->CDraw();
	heightmap->ColliderDraw();

	InterfaceObject3d::ReleaseCmdList();

	DrawLine3D::PreDraw(cmdList);
	player->LINEDraw();
	DrawLine3D::PostDraw();
}

void TestField::Finalize()
{
}

void TestField::ImguiDraw()
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

void TestField::GetConstbufferNum()
{
	//XMFLOAT3* inPos = new XMFLOAT3[max];
	//inPos = compute->GetConstBufferNum();
	//for (int i = 0; i < max; i++)
	//{
	//	nowPosition[i] = inPos[i];
	//}
}
