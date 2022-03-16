#include "MainEngine.h"
#include "Object3d.h"
#include "Sprite.h"
#include "DrawLine.h"
#include "DrawLine3D.h"
#include "Emitter.h"
#include "Fbx.h"
#include "NormalMap.h"
#include "SafeDelete.h"

MainEngine::~MainEngine()
{
	safe_delete(camera);
	safe_delete(postEffect);
}

//���C���ɏ����i�����������j
void MainEngine::Initialize(const wchar_t* gameName, int window_width, int window_height)
{
	//�E�B���h�E������
	winApp = WindowApp::GetInstance();
	winApp->Initialize(window_width, window_height, gameName);

	//1�t���[���̎��Ԑݒ�
	QueryPerformanceFrequency(&timeFreq);

	// 1�x�擾���Ă���(����v�Z�p)
	QueryPerformanceCounter(&timeStart);

	//directX������
	dXCommon = DirectXCommon::GetInstance();
	dXCommon->Initialize();

	//key
	input = Input::GetInstance();
	input->Initialize();

	xinput =XInputManager::GetInstance();
	xinput->Initialize();

	//�J�����̏�����
	camera = new Camera(window_width, window_height);

	//Object�n�̏�����
	Object3d::StaticInitialize(dXCommon->GetDevice(), camera);
	Sprite::StaticInitialize(dXCommon->GetDevice());
	DrawLine::StaticInitialize(dXCommon->GetDevice(), window_width, window_height);
	DrawLine3D::StaticInitialize(dXCommon->GetDevice());
	ParticleManager::Initialize(dXCommon->GetDevice(), dXCommon->GetCmdList());
	LightGroup::StaticInitialize(dXCommon->GetDevice());
	Fbx::StaticInitialize(dXCommon->GetDevice());
	NormalMap::StaticInitialize(dXCommon->GetDevice());

	//�Q�[���V�[��������
	scene = GameScene::GetInstance();
	scene->Initialize(camera);

	Sprite::LoadTexture(0, L"Resources/LetterResources/debugfont.png");

	postEffect = new PostEffect();
	postEffect->Initialize();

	//�[�x�̏�����
	dXCommon->CreateDepth();
}

//���C���ɏ����i�X�V�����j
bool MainEngine::Update()
{
	input->Update();

	//�G�X�P�[�v���~�������ꂽ�Ƃ��Q�[���I��
	if (input->PushKey(DIK_ESCAPE)|| GameFin() == true) { return true; }
	xinput->Update();

	return false;
}

//�f�o�b�O�p����
void MainEngine::DebugNum(float x, float y, float z)
{
	//�����̃f�o�b�O
	swprintf_s(str, L"%f,%f,%f\n", x, y, z);
	OutputDebugString(str);
}

//���C���ɏ����i�`�揈���j
void MainEngine::Draw()
{
	//�X�V
	scene->Update(camera);

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

//���C���ɏ����i�G�X�P�[�v�����͂��ꂽ��I�����鏈���j
bool MainEngine::GameFin() {
	//�~�������ꂽ�Ƃ�
	if (winApp->Update() == true) {
		return true;
	} else { return false; }

}