#include "MainEngine.h"
#include "DrawLine.h"
#include "DrawLine3D.h"
#include "Object3d.h"
#include "Sprite.h"
#include "DebugText.h"
#include "Emitter.h"
#include "Fbx.h"
#include "NormalMap.h"
#include "SafeDelete.h"
#include "ComputeShaderManager.h"
#include "GraphicsPipelineManager.h"

using namespace DirectX;
using namespace Microsoft::WRL;

MainEngine::~MainEngine()
{
	DebugText::Finalize();
	scene.reset();
	Object3d::Finalize();
	DrawLine3D::Finalize();
	DrawLine::Finalize();
	Sprite::Finalize();
	Fbx::Finalize();
	ParticleManager::Finalize();
	NormalMap::Finalize();
	postEffect->Finalize();
	ComputeShaderManager::Finalize();
}

void MainEngine::Initialize()
{
	//directX初期化
	dXCommon = DirectXCommon::Create();

	//key
	input = DirectInput::GetInstance();
	input->Initialize();

	//パッド
	Xinput = XInputManager::GetInstance();
	Xinput->Initialize();

	//Object系の初期化
	GraphicsPipelineManager::StaticInitialize(dXCommon->GetDevice());
	Object3d::StaticInitialize(dXCommon->GetDevice());
	Sprite::StaticInitialize(dXCommon->GetDevice());
	DrawLine::StaticInitialize(dXCommon->GetDevice());
	DrawLine3D::StaticInitialize(dXCommon->GetDevice());
	ParticleManager::StaticInitialize(dXCommon->GetDevice());
	LightGroup::StaticInitialize(dXCommon->GetDevice());
	Fbx::StaticInitialize(dXCommon->GetDevice());
	NormalMap::StaticInitialize(dXCommon->GetDevice());
	PostEffect::StaticInitialize();
	ComputeShaderManager::StaticInitialize(dXCommon->GetDevice());

	DebugText::GetInstance()->Initialize(0);

	scene = SceneManager::Create();

	postEffect = PostEffect::Create();

	fps = FrameRateKeep::Create();

	dXCommon->CreateCubeMap();
}

bool MainEngine::Update()
{
	input->Update();
	Xinput->Update();

	//エスケープか×が押されたときゲーム終了
	if (input->PushKey(DIK_ESCAPE)) { return true; }

	//更新
	scene->Update();

	return false;
}

void MainEngine::Draw()
{
	//描画
	postEffect->PreDrawScene(dXCommon->GetCmdList());
	dXCommon->CubeDraw();
	scene->Draw(dXCommon->GetCmdList());
	postEffect->PostDrawScene(dXCommon->GetCmdList());

	//描画前設定
	dXCommon->PreDraw();

	//imgui表示
	scene->ImguiDraw();

	//ポストエフェクト描画
	postEffect->Draw(dXCommon->GetCmdList());

	//コマンド実行
	dXCommon->PostDraw();
}

void MainEngine::debugNum(float x, float y, float z)
{
	//数字のデバッグ
	swprintf_s(str, L"%f,%f,%f\n", x, y, z);
	OutputDebugString(str);
}

void MainEngine::FrameControl(WindowApp* winApp)
{
	fps->FixedFps(winApp);
}