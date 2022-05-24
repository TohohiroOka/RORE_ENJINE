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
	//スプライト
	Sprite::LoadTexture("amm", "Resources/amm.jpg");

	sprite = Sprite::Create("amm");

	//モデル読み込み
	uma = Model::CreateFromOBJ("uma");
	ground = Model::CreateFromOBJ("wall");
	block = Model::CreateFromOBJ("Square");

	//プレイヤー生成
	PLAYER = Player::Create(uma.get());

	//地面生成
	GROUND = Ground::Create(ground.get());

	//触れられるオブジェクト生成
	std::unique_ptr<TouchableObject> Tobject3d = TouchableObject::Create(block.get());
	Tobject3d->SetScale(10.0f);
	Tobject3d->SetPosition({ -100,0,-100 });
	BLOCK = std::move(Tobject3d);

	//Fbxモデルの読み込み
	danceModel = FbxModel::Create("uma");
	SpherePBRModel = FbxModel::Create("SpherePBR");
	SpiralPBRModel = FbxModel::Create("SpiralPBR");

	//Fbxモデルオブジェクトの生成
	anm = Fbx::Create(SpiralPBRModel.get());
	anm->SetPosition({ 100,100,0 });
	anm->SetScale({ 15,15,15 });
	//anm->SetOutline(true);
	//anm->SetToon(true);

	//パーティクル用テクスチャの読み込み
	ParticleManager::LoadTexture("effect1", "Resources/particle/effect1.png");

	emit = Emitter::Create("effect1");

	//線
	line = DrawLine::Create();
	line_t = DrawLine::Create();
	line3d = DrawLine3D::Create(10);

	//コンピュートシェーダー生成
	compute = ComputeShaderManager::Create();
	for (int i = 0; i < max; i++)
	{
		startPosition[i] = { (float)10 * i,(float)10,(float)10 };//初期座標
		endPosition[i] = { (float)10,(float)10,(float)10 * i };//終了座標
		nowPosition[i] = startPosition[i];//現在座標
		time[i] = 0;
	}
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
	}

	if (input->PushKey(DIK_LEFT)) { cameraAngle++; }
	if (input->PushKey(DIK_RIGHT)) { cameraAngle--; }
	if (input->PushKey(DIK_UP)) { cameraY++; }
	if (input->PushKey(DIK_DOWN)) { cameraY--; }

	//Obj
	PLAYER->SetCameraAngle(cameraAngle);
	PLAYER->Update();
	GROUND->Update();
	BLOCK->Update();

	//Fbx
	anm->Update();

	//パーティクル
	emit->InEmitter(50, 30, { (float)(rand() % 5) - 100,0,(float)(rand() % 5) }, { 0,2,0 },
		{ 0,0,0 }, 50, 1, { 1,1,1,1 }, { 0,0,0,1 });

	//スプライト
	sprite->Update();
	emit->Update();

	////線
	//line->SetLine({ 300,300 }, { 700,700 }, { 1,1,1,1 }, 50);
	//line->Update();
	//line_t->SetLine({ 20,500 }, { 500,400 }, { 1,1,1,1 }, 50);
	//line_t->Update();
	//XMFLOAT3 S_pos[10];
	//XMFLOAT3 E_pos[10];
	//for (int i = 0; i < 10; i++)
	//{
	//	S_pos[i] = { 0,(float)20 * i,(float)i * 10 };
	//	E_pos[i] = { 0,(float)20 * i,(float)-i * 10 };
	//}
	//line3d->SetLine(S_pos, E_pos, 50);
	//line3d->SetColor({ 1,1,1,1 });
	//line3d->Update();

	//カメラ更新
	float cameraRadius = DirectX::XMConvertToRadians(cameraAngle);
	const float range = 50.0f;
	XMFLOAT3 cameraPos = PLAYER->GetPosition();
	cameraPos.y += 40.0f;
	if (input->PushKey(DIK_Z))
	{
		cameraPos.z -= 50;
	}
	camera->SetEye(cameraPos);
	camera->SetTarget({ cosf(cameraRadius) * range + cameraPos.x,cameraY,sinf(cameraRadius) * range + cameraPos.z });

	input = nullptr;
	xinput = nullptr;
}

void TestField::Draw()
{
	assert(cmdList);

	Object3d::PreDraw(cmdList);
	PLAYER->Draw();
	//GROUND->Draw();
	BLOCK->Draw();
	Object3d::PostDraw();

	Fbx::PreDraw(cmdList);
	anm->Draw();
	Fbx::PostDraw();

	//スプライト描画
	Sprite::PreDraw(cmdList);
	sprite->Draw();
	DebugText::GetInstance()->DrawAll(cmdList);
	Sprite::PostDraw();

	//線
	DrawLine::PreDraw(cmdList);
	line->Draw();
	line_t->Draw();
	DrawLine::PostDraw();

	//線3d
	DrawLine3D::PreDraw(cmdList);
	line3d->Draw();
	DrawLine3D::PostDraw();

	ParticleManager::PreDraw(cmdList);
	//パーティクル
	emit->Draw();
	ParticleManager::PostDraw();

	//コンピュートシェーダー
	compute->PreUpdate(cmdList);
	compute->ShaderUpdate(max, startPosition, endPosition, nowPosition, time);
	compute->PostUpdate();
}

void TestField::Finalize()
{
}

void TestField::ImguiDraw()
{
	float baseColor[3];//ベースカラ―
	float metalness;//金属度(0 or 1)
	float specular;//鏡面反射度
	float roughness;//粗さ

	baseColor[0] = anm->GetBaseColor().x;
	baseColor[1] = anm->GetBaseColor().y;
	baseColor[2] = anm->GetBaseColor().z;
	metalness = anm->GetMetalness();
	specular = anm->GetSpecular();
	roughness = anm->GetRoughness();

	//ライトon/off
	static bool lightAct1 = false;
	static bool lightAct2 = false;
	static bool lightAct3 = false;

	ImGui::Begin("Material");
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(300, 130));
	ImGui::ColorEdit3("baseColor", baseColor, ImGuiColorEditFlags_Float);
	ImGui::SliderFloat("metalness", &metalness, 0, 1);
	ImGui::SliderFloat("specular", &specular, 0, 1);
	ImGui::SliderFloat("roughness", &roughness, 0, 1);
	ImGui::Checkbox("Light1", &lightAct1);
	ImGui::Checkbox("Light2", &lightAct2);
	ImGui::Checkbox("Light3", &lightAct3);
	ImGui::End();

	anm->SetBaseColor({ baseColor[0],baseColor[1],baseColor[2] });
	anm->SetMetalness(metalness);
	anm->SetSpecular(specular);
	anm->SetRoughness(roughness);

	light->SetDirLightActive(0, lightAct1);
	light->SetDirLightActive(1, lightAct2);
	light->SetDirLightActive(2, lightAct3);
}

void TestField::GetConstbufferNum()
{
	XMFLOAT3* inPos = new XMFLOAT3[max];
	inPos = compute->GetConstBufferNum();
	for (int i = 0; i < max; i++)
	{
		nowPosition[i] = inPos[i];
	}
}
