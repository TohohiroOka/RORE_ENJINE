#include "MainEngine.h"
//#include "DrawLine.h"
#include "InstanceObject.h"
#include "DrawLine3D.h"
#include "InterfaceObject3d.h"
#include "Sprite.h"
#include "DebugText.h"
#include "Emitter.h"
//#include "Fbx.h"
#include "SafeDelete.h"
#include "ComputeShaderManager.h"
#include "GraphicsPipelineManager.h"
#include "Texture.h"
#include "HeightMap.h"
#include "WindowApp.h"

using namespace DirectX;
using namespace Microsoft::WRL;

MainEngine::~MainEngine()
{
	DebugText::Finalize();
	scene.reset();
	//DrawLine::Finalize();
	Sprite::Finalize();
	//Fbx::Finalize();
	CubeMap::Finalize();
	ParticleManager::Finalize();
	for (auto& i : postEffect) {
		i->Finalize();
	}
	ComputeShaderManager::Finalize();
	DescriptorHeapManager::Finalize();
}

void MainEngine::Initialize()
{
	//directX初期化
	dXCommon = DirectXCommon::Create();

	//key
	input = DirectInput::GetInstance();
	input->Initialize();

	//パッド
	xinput = XInputManager::GetInstance();
	xinput->Initialize();

	//Object系の初期化
	InstanceObject::StaticInitialize(dXCommon->GetDevice());
	GraphicsPipelineManager::SetDevice(dXCommon->GetDevice());
	InterfaceObject3d::StaticInitialize(dXCommon->GetDevice());
	Sprite::StaticInitialize(dXCommon->GetDevice());
	DrawLine3D::StaticInitialize(dXCommon->GetDevice());
	ParticleManager::SetDevice(dXCommon->GetDevice());
	LightGroup::StaticInitialize(dXCommon->GetDevice());
	//Fbx::StaticInitialize(dXCommon->GetDevice());
	PostEffect::StaticInitialize();
	ComputeShaderManager::StaticInitialize(dXCommon->GetDevice());
	DebugText::GetInstance()->Initialize();
	CubeMap::StaticInitialize(dXCommon->GetDevice());

	scene = SceneManager::Create();

	for (auto& i : postEffect) {
		i = PostEffect::Create();
	}
	fps = FrameRateKeep::Create();
}

bool MainEngine::Update()
{
	input->Update();
	xinput->Update();

	//エスケープか×が押されたときゲーム終了
	if (input->PushKey(DIK_ESCAPE)) { return true; }

	//更新
	scene->SetCameraNum(0);
	scene->Update();

	return false;
}

void MainEngine::Draw()
{
	//描画
	DescriptorHeapManager::PreDraw(dXCommon->GetCmdList());

	scene->SetCameraNum(0);
	postEffect[0]->PreDrawScene(dXCommon->GetCmdList());
	scene->Draw(dXCommon->GetCmdList());
	postEffect[0]->PostDrawScene(dXCommon->GetCmdList());

	scene->SetCameraNum(useCamera);
	postEffect[useCamera]->PreDrawScene(dXCommon->GetCmdList());
	scene->Draw(dXCommon->GetCmdList());
	postEffect[useCamera]->PostDrawScene(dXCommon->GetCmdList());
	useCamera++;
	if (useCamera == 7) {
		useCamera = 1;
	}

	//描画前設定
	dXCommon->PreDraw();

	//imgui表示
	scene->ImguiDraw();
	ImguiDraw();
	postEffect[0]->Draw(dXCommon->GetCmdList());
	//コマンド実行
	dXCommon->PostDraw();

	scene->FrameReset();
}

void MainEngine::ImguiDraw()
{
	const XMFLOAT2 window = { float(WindowApp::GetWindowWidth()) , float(WindowApp::GetWindowHeight()) };

	const float x_s = 305.0f;
	const float y_s = 295.0f;
	std::array<XMFLOAT2, 6> pos = { XMFLOAT2
		{0.0f,window.y - y_s},{x_s,window.y - y_s}, {x_s * 2.0f,window.y - y_s}, {x_s * 3.0f,window.y - y_s},
		{x_s * 4.0f,window.y - y_s},{x_s * 4.0f,window.y - y_s * 2.0f}
	};

	const std::array<std::string, 6> imName = { "buck","front","under","top","Right","Left" };

	for (int i = 1; i < 7; i++) {
		ImGui::Begin(imName[i-1].c_str());
		ImGui::SetWindowPos(ImVec2(pos[i - 1].x, pos[i - 1].y));
		ImGui::SetWindowSize(ImVec2(290.0f, 295.0f));
		ImGui::Image((ImTextureID)postEffect[i]->GetTex()->descriptor->gpu.ptr, ImVec2(window.x / 2.0f, window.y / 2.0f));
		ImGui::End();
	}
}

void MainEngine::FrameControl()
{
	fps->FixedFps();
}
