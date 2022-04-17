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

MainEngine::~MainEngine()
{
	safe_delete(camera);
	safe_delete(scene);
	DrawLine3D::AllDelete();
	DrawLine::AllDelete();
	Object3d::AllDelete();
	Sprite::AllDelete();
	Fbx::AllDelete();
	ParticleManager::AllDelete();
	NormalMap::AllDelete();
	postEffect->AllDelete();
	safe_delete(dXCommon);
	safe_delete(winApp);
}

void MainEngine::Initialize(const wchar_t* gameName, int window_width, int window_height)
{
	//�E�B���h�E������
	winApp = new WindowApp();
	winApp->Initialize(window_width, window_height, gameName);

	//1�t���[���̎��Ԑݒ�
	QueryPerformanceFrequency(&timeFreq);

	// 1�x�擾���Ă���(����v�Z�p)
	QueryPerformanceCounter(&timeStart);

	//directX������
	dXCommon = new DirectXCommon();
	dXCommon->Initialize();

	//key
	input = DirectInput::GetInstance();
	input->Initialize();

	//�p�b�h
	Xinput = XInputManager::GetInstance();
	Xinput->Initialize();

	//�J�����̏�����
	camera = new Camera();

	//Object�n�̏�����
	Object3d::StaticInitialize(dXCommon->GetDevice(), camera);
	Sprite::StaticInitialize(dXCommon->GetDevice());
	DrawLine::StaticInitialize(dXCommon->GetDevice());
	DrawLine3D::StaticInitialize(dXCommon->GetDevice());
	ParticleManager::Initialize(dXCommon->GetDevice());
	LightGroup::StaticInitialize(dXCommon->GetDevice());
	Fbx::StaticInitialize(dXCommon->GetDevice());
	NormalMap::StaticInitialize(dXCommon->GetDevice());
	PostEffect::StaticInitialize();
	ComputeShaderManager::StaticInitialize(dXCommon->GetDevice());

	scene = new GameScene();
	scene->Initialize();

	Sprite::LoadTexture(0, L"Resources/LetterResources/debugfont.png");

	postEffect = PostEffect::Create();

	//�[�x�̏�����
	dXCommon->CreateDepth();
}

bool MainEngine::Update()
{
	input->Update();
	Xinput->Update();

	//�G�X�P�[�v���~�������ꂽ�Ƃ��Q�[���I��
	if (input->PushKey(DIK_ESCAPE) || gameFin(winApp) == true) { return true; }

	//�X�V
	scene->Update(camera);

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

void MainEngine::frameRateKeep() {
	// ���̎��Ԃ��擾
	QueryPerformanceCounter(&timeEnd);
	// (���̎��� - �O�t���[���̎���) / ���g�� = �o�ߎ���(�b�P��)
	frameTime = static_cast<float>(timeEnd.QuadPart - timeStart.QuadPart) / static_cast<float>(timeFreq.QuadPart);

	if (frameTime < MIN_FREAM_TIME) { // ���Ԃɗ]�T������
		// �~���b�ɕϊ�
		DWORD sleepTime = static_cast<DWORD>((MIN_FREAM_TIME - frameTime) * 1000);

		timeBeginPeriod(1); // ����\���グ��(�������Ȃ���Sleep�̐��x�̓K�^�K�^)
		Sleep(sleepTime);   // �Q��
		timeEndPeriod(1);   // �߂�

		// ���T�Ɏ����z��(�������Ȃ���fps���ςɂȂ�?)
		return;
	}

	if (frameTime > 0.0) { // �o�ߎ��Ԃ�0���傫��(�������Ȃ��Ɖ��̌v�Z�Ń[�����Z�ɂȂ�Ǝv���)
		fps = (fps * 0.99f) + (0.01f / frameTime); // ����fps���v�Z
#ifdef _DEBUG
// �f�o�b�O�p(�f�o�b�K��FSP�o��)
#ifdef UNICODE
		std::wstringstream stream;
#else
		std::stringstream stream;
#endif
		stream << fps << " FPS" << std::endl;
		// �J�E���^�t����10���1��o�́A�Ƃ��ɂ��Ȃ��ƌ��Â炢������
		OutputDebugString(stream.str().c_str());
#endif // _DEBUG
	}

	timeStart = timeEnd; // ����ւ�
}

//���C���ɏ����i�G�X�P�[�v�����͂��ꂽ��I�����鏈���j
bool MainEngine::gameFin(WindowApp* winApp) {
	//�~�������ꂽ�Ƃ�
	if (winApp->Update() == true) {
		return true;
	} else { return false; }

}