#include "GameScene.h"
#include "MainEngine.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"
#include "Camera.h"

#include <cassert>
#include <sstream>
#include <iomanip>

std::unique_ptr<GameScene> GameScene::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	GameScene* instance = new GameScene();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();

	return std::unique_ptr<GameScene>(instance);
}

void GameScene::Initialize()
{
	//サウンド用
	audio = std::make_unique<Audio>();

	//ライト
	light = LightGroup::Create();
	//light->DefaultLightSetting();
	light->SetPointLightActive(0, true);
	light->SetPointLightPos(0, { -100,0,-100 });
	light->SetPointLightColor(0, { 1,1,1 });
	light->SetPointLightAtten(0, { 0.001f,0.00023f,0.000001f });
	// 3Dオブエクトにライトをセット
	Object3d::SetLightGroup(light.get());

	//スプライト
	Sprite::LoadTexture(1, L"Resources/amm.jpg");

	sprite = Sprite::Create(1);

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

	//NormalMapテクスチャの読み込み
	tex[0] = NormalMap::LoadTexture(L"Resources/white1x1.png");
	tex[1] = NormalMap::LoadTexture(L"Resources/wN1.jpg");
	tex[2] = NormalMap::LoadTexture(L"Resources/pN2.png");

	//水オブジェクト生成
	water= NormalMap::Create();
	water->SetPosition({ 100,5,100 });
	water->SetScale({ 2,2,2 });
	water->SetMainTexColor({ 1.0f,1.0f,1.0f,1.0f });
	water->SetSubTexColor1({ 0.1f, 0.7f, 1.0f, 0.5f });
	water->SetSubTexColor2({ 1.0f, 1.0f, 1.0f, 0.5f });

	//Fbxモデルの読み込み
	danceModel = FbxModel::Create("uma");

	//Fbxモデルオブジェクトの生成
	anm = Fbx::Create(danceModel.get());
	anm->SetScale({ 10,10,10 });
	anm->SetAnimation(true);
	anm->SetOutline(true);
	anm->SetToon(true);

	//パーティクル用テクスチャの読み込み
	ParticleManager::LoadTexture(0, L"Resources/particle/effect1.png");

	emit = Emitter::Create(0);

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

void GameScene::Update(Camera* camera)
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//カメラのセット
	Object3d::SetCamera(camera);
	NormalMap::SetCamera(camera);
	Fbx::SetCamera(camera);
	DrawLine3D::SetCamera(camera);
	ParticleManager::SetCamera(camera);

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
	water->SetUvPosition(uvPos);
	water->SetLightPosition(lightPos);
	water->Update();

	//Fbx
	anm->Update();

	//パーティクル
	emit->InEmitter(50, 30, { (float)(rand() % 5)-20,0,(float)(rand() % 5) }, { 0,2,0 },
		{ 0,0,0 }, 50, 1, { 1,1,1,1 }, { 0,0,0,1 });

	//ライト
	light->Update();
	// 3Dオブエクトにライトをセット
	Object3d::SetLightGroup(light.get());
	Fbx::SetLightGroup(light.get());

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
	camera->SetPosition(PLAYER->GetPosition());
	camera->TpsCamera({ 0,50,-100 });
	camera->Update();

	input = nullptr;
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

void GameScene::GetConstbufferNum()
{
	XMFLOAT3* inPos = new XMFLOAT3[max];
	inPos = compute->GetConstBufferNum();
	for (int i = 0; i < max; i++)
	{
		nowPosition[i] = inPos[i];
	}
}