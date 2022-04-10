#include "MainEngine.h"
#include "Object3d.h"
#include "Sprite.h"
#include "DrawLine.h"
#include "DrawLine3D.h"
#include "Emitter.h"
#include "Fbx.h"
#include "NormalMap.h"
#include "SafeDelete.h"
#include "ComputeShaderManager.h"

MainEngine::~MainEngine()
{
	winApp->Release();
	safe_delete(winApp);
}

void MainEngine::AllDelete()
{
	safe_delete(scene);
	safe_delete(camera);
	safe_delete(postEffect);
	safe_delete(dXCommon);
	safe_delete(audio);
	safe_delete(dXCommon);
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
	xinput = XInputManager::GetInstance();
	xinput->Initialize();

	//�J�����̏�����
	camera = new Camera();

	//Object�n�̏�����
	Object3d::StaticInitialize(dXCommon->GetDevice(), camera);
	Sprite::StaticInitialize(dXCommon->GetDevice());
	DrawLine::StaticInitialize(dXCommon->GetDevice(), window_width, window_height);
	DrawLine3D::StaticInitialize(dXCommon->GetDevice());
	ParticleManager::Initialize(dXCommon->GetDevice(), dXCommon->GetCmdList());
	LightGroup::StaticInitialize(dXCommon->GetDevice());
	Fbx::StaticInitialize(dXCommon->GetDevice());
	NormalMap::StaticInitialize(dXCommon->GetDevice());
	ComputeShaderManager::StaticInitialize(dXCommon->GetDevice());
	//�Q�[���V�[��������
	scene = new GameScene();
	scene->Initialize();

	//�f�o�b�O�e�L�X�g�̃e�N�X�`��
	Sprite::LoadTexture(0, L"Resources/LetterResources/debugfont.png");

	//�|�X�g�G�t�F�N�g������
	postEffect = new PostEffect();
	postEffect->Initialize();

	//Audio������
	audio = new Audio();

	//�[�x�̏�����
	dXCommon->CreateDepth();
}

bool MainEngine::Update()
{
	input->Update();

	//�G�X�P�[�v���~�������ꂽ�Ƃ��Q�[���I��
	if (input->PushKey(DIK_ESCAPE)|| GameFin() == true) { return true; }
	xinput->Update();

	//�X�V
	scene->Update(audio, camera);

	return false;
}

void MainEngine::DebugNum(float x, float y, float z)
{
	//�����̃f�o�b�O
	swprintf_s(str, L"%f,%f,%f\n", x, y, z);
	OutputDebugString(str);
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

	scene->GetConstbufferNum();
}

void MainEngine::FrameRateKeep() {
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

bool MainEngine::GameFin() {
	//�~�������ꂽ�Ƃ�
	if (winApp->Update() == true) {
		return true;
	} else { return false; }

}