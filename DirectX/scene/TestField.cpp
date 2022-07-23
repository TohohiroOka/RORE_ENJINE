#include "TestField.h"
#include "SceneManager.h"
#include "BrowsingCircle.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"
#include "PostEffect.h"

#include <imgui.h>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <memory>

void TestField::Initialize()
{
	playerModel = Model::CreateFromOBJ("uma");
	player = Player::Create(playerModel.get());
	heightmap = HeightMap::Create("heightmap01.bmp", "Dirt.jpg", "Grass.jpg");
}

void TestField::Update()
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//シーンの移行
	if (input->TriggerKey(DIK_0))
	{
		BrowsingCircle* nextScene = new BrowsingCircle();
		nextScene->Initialize();
		SceneManager::SetNextScene(nextScene);
		return;
	}

	if (input->PushKey(DIK_LEFT)) { cameraAngle+=3.0f; }
	if (input->PushKey(DIK_RIGHT)) { cameraAngle-= 3.0f; }
	if (input->PushKey(DIK_UP)) { cameraY-= 3.0f; }
	if (input->PushKey(DIK_DOWN)) { cameraY+= 3.0f; }

	if (cameraAngle > 360 || cameraAngle < -360)
	{
		cameraAngle = 0;
	}

	//ラジアン変換
	float radiusLR = DirectX::XMConvertToRadians(cameraAngle + 90.0f);
	float radiusUD = DirectX::XMConvertToRadians(cameraAngle);

	//player移動
	float Pspeed = 5.0f;

	//右入力
	if (input->PushKey(DIK_A)) {
		cameraPos.x += Pspeed * cosf(radiusLR);
		cameraPos.z += Pspeed * sinf(radiusLR);
	}
	//左入力
	if (input->PushKey(DIK_D)) {
		cameraPos.x -= Pspeed * cosf(radiusLR);
		cameraPos.z -= Pspeed * sinf(radiusLR);
	}
	if (input->PushKey(DIK_W)) {
		cameraPos.x += Pspeed * cosf(radiusUD);
		cameraPos.z += Pspeed * sinf(radiusUD);
	}
	//左入力
	if (input->PushKey(DIK_S)) {
		cameraPos.x -= Pspeed * cosf(radiusUD);
		cameraPos.z -= Pspeed * sinf(radiusUD);
	}

	if (input->PushKey(DIK_Z)) {
		cameraPos.y += 3.0f;
	}
	//左入力
	if (input->PushKey(DIK_X)) {
		cameraPos.y -= 3.0f;
	}

	player->Update();
	heightmap->Update();

	float cameraRadius = DirectX::XMConvertToRadians(cameraAngle);
	const float range = 100.0f;
	camera->SetEye(cameraPos);
	XMFLOAT3 cameraT = { cosf(cameraRadius) * range + cameraPos.x,cameraPos.y + cameraY,sinf(cameraRadius) * range + cameraPos.z };
	camera->SetTarget(cameraT);

	player->SetPosition(cameraT);

	input = nullptr;
	xinput = nullptr;
}

bool TestField::Draw()
{
	assert(cmdList);

	if (drawPlayer)
	{
		Object3d::PreDraw(cmdList);
		player->Draw();
		Object3d::PostDraw();
	}

	HeightMap::PreDraw(cmdList);
	heightmap->Draw(0);
	HeightMap::PostDraw();

	return false;
}

void TestField::Finalize()
{
}

void TestField::ImguiDraw()
{
	bool Fog;
	bool Bloom;//ブルーム
	bool Toon;//トゥーン
	bool OutLine;//アウトライン
	float OutlineWidth;//アウトラインの幅
	float OutlineColor[3];//アウトラインの色

	Fog = PostEffect::GetFog();
	Bloom = player->GetBloom();
	Toon = player->GetToon();
	OutLine = player->GetOutline();
	OutlineWidth = player->GetOutlineWidth();
	OutlineColor[0] = player->GetOutlineColor().x;
	OutlineColor[1] = player->GetOutlineColor().y;
	OutlineColor[2] = player->GetOutlineColor().z;

	ImGui::Begin("PostEffect");
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(300, 400));
	ImGui::Checkbox("Fog", &Fog);
	ImGui::Checkbox("drawUma", &drawPlayer);
	ImGui::Checkbox("Bloom", &Bloom);
	ImGui::Checkbox("Toon", &Toon);
	ImGui::Checkbox("OutLine", &OutLine);
	ImGui::SliderFloat("OutLineWidth", &OutlineWidth, 0, 0.005f);
	ImGui::ColorEdit3("OutLinColor", OutlineColor, ImGuiColorEditFlags_Float);
	ImGui::End();

	PostEffect::SetFog(Fog);
	player->SetBloom(Bloom);
	player->SetToon(Toon);
	player->SetOutline(OutLine);
	player->SetOutlineWidth(OutlineWidth);
	player->SetOutlineColor({ OutlineColor[0],OutlineColor[1],OutlineColor[2],1.0f });
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
