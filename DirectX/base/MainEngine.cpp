#include "MainEngine.h"
#include "DrawLine.h"
#include "DrawLine3D.h"
#include "GameScene.h"
#include "Object3d.h"
#include "Sprite.h"
#include "Emitter.h"
#include "Fbx.h"
#include "NormalMap.h"
#include "SafeDelete.h"
#include "ComputeShaderManager.h"
#include "GraphicsPipelineManager.h"
#include <chrono>

using namespace DirectX;
using namespace Microsoft::WRL;

MainEngine::~MainEngine()
{
	DebugText::GetInstance()->Finalize();
	scene.reset();
	Object3d::Finalize();
	DrawLine3D::Finalize();
	DrawLine::Finalize();
	Sprite::Finalize();
	Fbx::Finalize();
	ParticleManager::Finalize();
	NormalMap::Finalize();
	postEffect->Finalize();
}

void MainEngine::Initialize()
{
	//directX������
	dXCommon = DirectXCommon::Create();

	//key
	input = DirectInput::GetInstance();
	input->Initialize();

	//�p�b�h
	Xinput = XInputManager::GetInstance();
	Xinput->Initialize();

	//�J�����̏�����
	camera = Camera::Create();

	//Object�n�̏�����
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

	scene = GameScene::Create();

	postEffect = PostEffect::Create();

	fps = FrameRateKeep::Create();
}

bool MainEngine::Update()
{
	input->Update();
	Xinput->Update();

	//�G�X�P�[�v���~�������ꂽ�Ƃ��Q�[���I��
	if (input->PushKey(DIK_ESCAPE)) { return true; }

	//�X�V
	scene->Update(camera.get());

	return false;
}

void MainEngine::Draw()
{
	//�`��
	postEffect->PreDrawScene(dXCommon->GetCmdList());
	scene->Draw(dXCommon->GetCmdList());
	postEffect->PostDrawScene(dXCommon->GetCmdList());

	//�`��O�ݒ�
	dXCommon->BeforeDraw();

	//�|�X�g�G�t�F�N�g�`��
	postEffect->Draw(dXCommon->GetCmdList());

	//�R�}���h���s
	dXCommon->AfterDraw();
}

void MainEngine::debugNum(float x, float y, float z)
{
	//�����̃f�o�b�O
	swprintf_s(str, L"%f,%f,%f\n", x, y, z);
	OutputDebugString(str);
}

void MainEngine::FrameControl(WindowApp* winApp)
{
	fps->FixedFps(winApp);
}