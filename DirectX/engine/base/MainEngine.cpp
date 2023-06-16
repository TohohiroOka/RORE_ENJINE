#include "MainEngine.h"
#include "InstanceObject.h"
#include "Sprite.h"
#include "DebugText.h"
#include "Emitter.h"
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
	ParticleManager::Finalize();
	postEffect->Finalize();
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
	GraphicsPipelineManager::SetDevice(dXCommon->GetDevice());
	ObjectBase::StaticInitialize(dXCommon->GetDevice());
	LightGroup::StaticInitialize(dXCommon->GetDevice());
	ComputeShaderManager::StaticInitialize(dXCommon->GetDevice());
	DebugText::GetInstance()->Initialize();

	scene = SceneManager::Create();

	postEffect = PostEffect::Create();

	for (int i = 0; i < int(BasePostEffect::EffectTyep::size); i++) {
		basePost[i] = BasePostEffect::Create(BasePostEffect::EffectTyep(i));
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
	scene->Update();

	return false;
}

void MainEngine::Draw()
{
	//描画
	DescriptorHeapManager::PreDraw(dXCommon->GetCmdList());
	ObjectBase::SetCmdList(dXCommon->GetCmdList());

	postEffect->PreDrawScene();
	scene->Draw(dXCommon->GetCmdList());
	postEffect->PostDrawScene();

	basePost[int(BasePostEffect::EffectTyep::bloom)]->PreDrawScene();
	basePost[int(BasePostEffect::EffectTyep::bloom)]->Draw(postEffect->GetTex(PostEffect::TexType::bloom));
	basePost[int(BasePostEffect::EffectTyep::bloom)]->PostDrawScene();

	basePost[int(BasePostEffect::EffectTyep::outline)]->PreDrawScene();
	basePost[int(BasePostEffect::EffectTyep::outline)]->Draw(postEffect->GetTex(PostEffect::TexType::outline));
	basePost[int(BasePostEffect::EffectTyep::outline)]->PostDrawScene();

	//描画前設定
	dXCommon->PreDraw();

	//imgui表示
	scene->ImguiDraw();
	ImguiDraw();
	std::vector<Texture*> postTex(4);
	postTex[0] = postEffect->GetTex(PostEffect::TexType::normal);
	postTex[1] = basePost[int(BasePostEffect::EffectTyep::bloom)]->GetTex();
	postTex[2] = basePost[int(BasePostEffect::EffectTyep::outline)]->GetTex();
	postTex[3] = postEffect->GetTex(PostEffect::TexType::depth);

	postEffect->Draw(postTex);
	//コマンド実行
	dXCommon->PostDraw();

	scene->FrameReset();

	ObjectBase::ReleaseCmdList();
	GraphicsPipelineManager::ResetDrawSet();
}

void MainEngine::ImguiDraw()
{
}

void MainEngine::FrameControl()
{
	fps->FixedFps();
}
