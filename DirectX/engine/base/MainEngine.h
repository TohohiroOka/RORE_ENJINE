#pragma once
#include "DirectXCommon.h"
#include "SceneManager.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "PostEffect.h"
#include "FrameRateKeep.h"
#include "CubeMap.h"

#include <sstream>
#include <iomanip>
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")

class WindowApp;

class MainEngine
{
public:

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
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// �t���[�����[�g�L�[�v
	/// </summary>
	void FrameControl();

private:

	//DirectXCommon�̃C���X�^���X
	std::unique_ptr<DirectXCommon> dXCommon = nullptr;
	//Input�̃C���X�^���X
	DirectInput* input = nullptr;
	//XInputManager
	XInputManager* xinput = nullptr;
	//GameScene�̃C���X�^���X
	std::unique_ptr<SceneManager> scene = nullptr;
	//�|�X�g�G�t�F�N�g�̃C���X�^���X
	std::unique_ptr<PostEffect> postEffect = nullptr;
	//Fps�Œ�p�N���X�̃C���X�^���X
	std::unique_ptr<FrameRateKeep> fps = nullptr;
	//�L���[�u�}�b�v
	//std::unique_ptr<CubeMap> cubemap;
	//�����\���f�o�b�O�p
	wchar_t str[256] = {};
};