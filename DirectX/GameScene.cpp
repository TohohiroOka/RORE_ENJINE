#include "GameScene.h"
#include "MainEngine.h"
#include "SafeDelete.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "Audio.h"
#include "Camera.h"

#include <cassert>
#include <sstream>
#include <iomanip>

//ゲームで使用するクラス宣言
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"

const float radian = XM_PI / 180.0f;//ラジアン

GameScene::~GameScene()
{
	//newオブジェクトの破棄
	safe_delete(sprite);
	safe_delete(emit);
	safe_delete(uma);
	safe_delete(ground);
	safe_delete(block);
	safe_delete(PLAYER);
	safe_delete(GROUND);
	safe_delete(BLOCK);
	safe_delete(water);
	safe_delete(light);
	safe_delete(anm);
	safe_delete(line);
	safe_delete(line_t);
	//text->AllDelete();
	for (int i = 0; i < 10; i++)
	{
		safe_delete(line3d[i]);
	}
}

void GameScene::Initialize()
{
	//ライト
	light = LightGroup::Create();
	light->DefaultLightSetting();
	// 3Dオブエクトにライトをセット
	Object3d::SetLightGroup(light);

	//スプライト
	Sprite::LoadTexture(1, L"Resources/amm.jpg");

	//text = DebugText::GetInstance();
	//text->Initialize(0);

	sprite = Sprite::Create();
	sprite->SetTexNumber(1);

	//object3d
	uma = Model::CreateFromOBJ("uma");
	ground = Model::CreateFromOBJ("wall");
	block = Model::CreateFromOBJ("Square");

	PLAYER = Player::Create(uma);
	
	GROUND = Ground::Create(ground);
	GROUND->SetScale(100);
	GROUND->SetPosition({ 1, -5, 0 });
	
	TouchableObject* Tobject3d = TouchableObject::Create(block);
	Tobject3d->SetScale(10.0f);
	Tobject3d->SetPosition({ -100,0,-100 });
	BLOCK = Tobject3d;

	//NormalMap
	tex[0] = NormalMap::LoadTexture(L"Resources/white1x1.png");
	tex[1] = NormalMap::LoadTexture(L"Resources/wN1.jpg");
	tex[2] = NormalMap::LoadTexture(L"Resources/pN2.png");

	water = new NormalMap();
	water->Create();

	//Fbx
	FbxUma = Fbx::LoadFbx("uma");

	anm = new Fbx();
	anm->CreateModel(FbxUma);

	//パーティクル用
	ParticleManager::LoadTexture(0, L"Resources/particle/effect1.png");

	emit = new Emitter();
	emit->Create(0);

	//線
	line = new DrawLine();
	line = DrawLine::Create();
	line_t = new DrawLine();
	line_t = DrawLine::Create();
	for (int i = 0 ; i < 10; i++)
	{
		line3d[i] = new DrawLine3D();
		line3d[i] = DrawLine3D::Create();
	}
}

void GameScene::Update(Audio* audio, Camera* camera)
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//Obj
	PLAYER->Update();
	GROUND->Update();
	BLOCK->Update();

	//NormalMap
	XMFLOAT4 normal = { 1.0f,1.0f,1.0f,1.0f };
	//法線マップの移動処理
	if (lightF) { lightPos -= 0.5f; } else if (lightF == false) { lightPos += 0.5f; }

	if (lightPos > 30) { lightF = true; }
	if (lightPos < -30) { lightF = false; }

	uvPos += 0.2f;
	water->SetInformation({ 100,5,100 }, { 0,0,0 }, { 2,2,2 },
		{ 1.0f,1.0f,1.0f,1.0f }, { 0.1f, 0.7f, 1.0f, 0.5f }, { 1.0f, 1.0f, 1.0f, 0.5f }, lightPos, uvPos);
	water->Update(camera);

	//Fbx
	anm->SetInformation({ 0,0,0 }, { 0,0,0 }, { 10,10,10 }, true);
	anm->Update(camera);

	//パーティクル
	emit->InEmitter(50, 30, { (float)(rand() % 5)-20,0,(float)(rand() % 5) }, { 0,2,0 },
		{ 0,0,0 }, 50, 1, { 1,1,1,1 }, { 0,0,0,1 });

	//ライト
	light->Update();

	//スプライト
	sprite->Update();
	emit->Update(camera);

	////線
	line->SetLine({ 300,300 }, { 700,700 }, { 1,1,1,1 }, 50);
	line->Update();
	line_t->SetLine({ 20,500 }, { 500,400 }, { 1,1,1,1 }, 50);
	line_t->Update();
	for (int i = 0; i < 10; i++)
	{
		line3d[i]->SetLine({ 0,(float)20 * i,(float)i * 10 }, { 0,(float)20 * i,(float)-i * 10 }, { 1,1,1,1 }, 50);
		line3d[i]->Update(camera);
	}

	//カメラ更新
	camera->SetPosition(PLAYER->GetPosition());
	camera->TpsCamera({ 0,0,-100 });
	camera->Update();

	time++;
	if (time % 50 < 25)
	{
		xinput->StartVibration();
	}
	else { xinput->EndVibration(); }

	xinput = nullptr;
}

void GameScene::Draw(ID3D12GraphicsCommandList* cmdList)
{
	NormalMap::PreDraw(cmdList);
	water->Draw(tex[0], tex[1], tex[2]);
	NormalMap::PostDraw();

	Object3d::PreDraw(cmdList);
	PLAYER->Draw();
	GROUND->Draw();
	BLOCK->Draw();
	Object3d::PostDraw();

	Fbx::PreDraw(cmdList);
	anm->Draw(FbxUma);
	Fbx::PostDraw();

	////スプライト描画
	//Sprite::PreDraw(cmdList);
	//sprite->Draw();
	//Sprite::PostDraw();

	//線
	DrawLine::PreDraw(cmdList);
	line->Draw();
	line_t->Draw();
	DrawLine::PostDraw();
	
	//線3d
	DrawLine3D::PreDraw(cmdList);
	for (int i = 0; i < 10; i++)
	{
		line3d[i]->Draw();
	}
	DrawLine3D::PostDraw();

	//パーティクル
	emit->Draw();

}
