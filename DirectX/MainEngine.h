#pragma once
#include "WindowApp.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "Input.h"
#include "XInputManager.h"
#include "Singleton.h"
#include "PostEffect.h"

#include <mmsystem.h>
#include<sstream>
#include <iomanip>

#pragma comment(lib,"winmm.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

class MainEngine : public Singleton< MainEngine >
{
	friend Singleton< MainEngine >;

private://�R���X�g���N�^&�f�X�g���N�^

	MainEngine() {};
	~MainEngine();

public://�����o�֐�

	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="gameName">�^�C�g���o�[��</param>
	/// <param name="window_width">��ʕ�</param>
	/// <param name="window_height">��ʍ���</param>
	void Initialize(const wchar_t* gameName, int window_width, int window_height);

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <returns>�Q�[���𑱂��邩</returns>
	bool Update();

	/// <summary>
	/// �f�o�b�O�p����
	/// </summary>
	/// <param name="x">�l1</param>
	/// <param name="y">�l2</param>
	/// <param name="z">�l3</param>
	void DebugNum(float x, float y, float z);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// �t���[�����[�g�Œ�
	/// </summary>
	void FrameRateKeep();

	/// <summary>
	/// �G�X�P�[�v�����͂��ꂽ��I�����鏈��
	/// </summary>
	/// <param name="winApp">�E�B���h�E�Y�C���X�^���X</param>
	/// <returns>�Q�[���𑱂��邩</returns>
	bool GameFin();

	/// <summary>
	/// Camera��getter
	/// </summary>
	/// <returns>Camera�̃C���X�^���X</returns>
	Camera* GetCamera() { return camera; }

private:

	//WindowApp�̃C���X�^���X
	WindowApp* winApp;
	//DirectXCommon�̃C���X�^���X
	DirectXCommon* dXCommon;
	//Input�̃C���X�^���X
	Input* input;
	//XInput�̃C���X�^���X
	XInputManager* xinput;
	//GameScene�̃C���X�^���X
	GameScene* scene;
	//�|�X�g�G�t�F�N�g
	PostEffect* postEffect = nullptr;
	//Camera�̃C���X�^���X
	Camera* camera = nullptr;
	//�����\���f�o�b�O�p
	wchar_t str[256] = {};
	//�t���[�����[�g�Œ�p
	const float MIN_FREAM_TIME = 1.0f / 60;
	//�t���[���̃J�E���g
	float frameTime = 0;
	//�t���[���n��
	LARGE_INTEGER timeStart;
	//�t���[���Ō�
	LARGE_INTEGER timeEnd;
	//���݃t���[��
	LARGE_INTEGER timeFreq;
	//����FPS
	float fps = 0;

private:

	XMFLOAT2 scale = { 1,1 };
	bool F[2] = { false,true };
};