#include "Title.h"
#include "SceneManager.h"
#include "Boss1.h"
#include "DirectInput.h"
#include <imgui.h>
#include "DebugText.h"

using namespace DirectX;

void Title::Initialize()
{
	Sprite::LoadTexture("amm", "Resources/amm.jpg", true);
	sp = Sprite::Create("amm");
	sp->SetPosition({ 500,500 });
	sp->SetTexSize({ 1000, 1000 });
	sp->SetSize({ 100,100 });
	sp->Update();

}

void Title::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	//�V�[���̈ڍs
	if (input->TriggerKey(DIK_RETURN))
	{
		Boss1* nextScene = new Boss1();
		SceneManager::SetNextScene(nextScene);
	}

	DebugText* text = DebugText::GetInstance();
	text->Print("push enter", 100, 200, 5.0f);
	text = nullptr;
}

void Title::Draw()
{
	Sprite::PreDraw(cmdList);
	sp->Draw();
	DebugText::GetInstance()->DrawAll();
	Sprite::PostDraw();
}

void Title::Finalize()
{
}

void Title::ImguiDraw()
{
	//float baseColor[3];//�x�[�X�J���\
	//float metalness;//�����x(0 or 1)
	//float specular;//���ʔ��˓x
	//float roughness;//�e��

	//baseColor[0] = circle->GetBaseColor().x;
	//baseColor[1] = circle->GetBaseColor().y;
	//baseColor[2] = circle->GetBaseColor().z;
	//metalness = circle->GetMetalness();
	//specular = circle->GetSpecular();
	//roughness = circle->GetRoughness();

	////���C�gon/off
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

void Title::GetConstbufferNum()
{
}
