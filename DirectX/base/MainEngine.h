#pragma once
#include "DirectXCommon.h"
#include "GameScene.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "PostEffect.h"
#include "Camera.h"

#include <sstream>
#include <iomanip>
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")

class MainEngine
{
public://���C���ɏ���

	MainEngine() = default;
	~MainEngine();

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize();

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
	void debugNum(float x, float y, float z);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// �t���[�����[�g�Œ�
	/// </summary>
	void frameRateKeep();

private:

	//DirectXCommon�̃C���X�^���X
	DirectXCommon* dXCommon = nullptr;
	//Input�̃C���X�^���X
	DirectInput* input = nullptr;
	//XInputManager
	XInputManager* Xinput = nullptr;
	//Camera�̃C���X�^���X
	Camera* camera = nullptr;
	//GameScene�̃C���X�^���X
	GameScene* scene = nullptr;
	//�|�X�g�G�t�F�N�g�̃C���X�^���X
	std::unique_ptr<PostEffect> postEffect = nullptr;
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
};