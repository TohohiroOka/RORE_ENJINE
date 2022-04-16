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
	//audioの解放
	safe_delete(audio);

	//ライトの解放
	safe_delete(light);

	//スプライトの解放
	safe_delete(sprite);

	//パーティクルの解放
	safe_delete(emit);

	//Modelの解放
	safe_delete(uma);
	safe_delete(ground);
	safe_delete(block);

	//Objの解放
	safe_delete(PLAYER);
	safe_delete(GROUND);
	safe_delete(BLOCK);
	safe_delete(circle);

	//normalMapの解放
	safe_delete(water);

	//Fbxの解放
	safe_delete(anm);

	//lineの解放
	safe_delete(line3d);

	//コンピュートシェーダーの解放
	safe_delete(compute);

	safe_delete(circle);
	safe_delete(cannonball);
}

void GameScene::Initialize()
{
	//サウンド用
	audio = new Audio();

	//ライト
	light = LightGroup::Create();
	// 3Dオブエクトにライトをセット
	Object3d::SetLightGroup(light);

	//スプライト
	Sprite::LoadTexture(1, L"Resources/amm.jpg");

	sprite = Sprite::Create(1);

	//object3d
	uma = Model::CreateFromOBJ("uma");
	ground = Model::CreateFromOBJ("wall");
	block = Model::CreateFromOBJ("Square");

	PLAYER = Player::Create(uma);
	
	circle = FreeFallCircle::Create(block);
	cannonball = Cannonball::Create(block);

	GROUND = Ground::Create(ground);
	GROUND->SetScale(100);
	GROUND->SetPosition({ 1, -5, 0 });
	
	TouchableObject* Tobject3d = TouchableObject::Create(block);
	Tobject3d->SetScale(10.0f);
	BLOCK = Tobject3d;
	BLOCK->SetPosition({ -10, 0, 0 });

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
	line3d = new DrawLine3D();
	line3d = DrawLine3D::Create(10);

	compute = new ComputeShaderManager();
	compute->Initialize();
	for (int i = 0; i < max; i++)
	{
		startPosition[i] = { (float)10 * i,(float)10,(float)10 };//初期座標
		endPosition[i] = { (float)10,(float)10,(float)10 * i };//終了座標
		nowPosition[i] = startPosition[i];//現在座標
		time[i] = 0;
	}
}

void GameScene::Update(Camera* camera)
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//Obj
	PLAYER->Update();
	GROUND->Update();
	BLOCK->Update();
	circle->Update();
	cannonball->Update();

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
	XMFLOAT3 S_pos[10];
	XMFLOAT3 E_pos[10];
	for (int i = 0; i < 10; i++)
	{
		S_pos[i] = { 0,(float)20 * i,(float)i * 10 };
		E_pos[i] = { 0,(float)20 * i,(float)-i * 10 };
	}
	line3d->SetLine(S_pos, E_pos, 50);
	line3d->SetColor({ 1,1,1,1 });
	line3d->Update(camera);

	//カメラ更新
	camera->SetPosition(PLAYER->GetPosition());
	camera->TpsCamera({ 0,0,-100 });
	camera->Update();

	xinput = nullptr;
}

void GameScene::Draw(ID3D12GraphicsCommandList* cmdList)
{
	NormalMap::PreDraw(cmdList);
	water->Draw(tex[0], tex[1], tex[2]);
	NormalMap::PostDraw();

	Object3d::PreDraw(cmdList);
	PLAYER->Draw();
	circle->Draw();
	cannonball->Draw();
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

	//線3d
	DrawLine3D::PreDraw(cmdList);
	line3d->Draw();
	DrawLine3D::PostDraw();

	ParticleManager::PreDraw(cmdList);
	//パーティクル
	emit->Draw();
	ParticleManager::PostDraw();


	compute->PreUpdate(cmdList);
	compute->ShaderUpdate(max, startPosition, endPosition, nowPosition, time);
	compute->PostUpdate();
}

void GameScene::GetConstbufferNum()
{
	XMFLOAT3* inPos = new XMFLOAT3[max];
	inPos = compute->GetConstBufferNum();
	for (int i = 0; i < max; i++)
	{
		nowPosition[i] = inPos[i];
	}
}