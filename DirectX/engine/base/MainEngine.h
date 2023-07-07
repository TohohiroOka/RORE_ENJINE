#pragma once
#include "DirectXCommon.h"
#include "SceneManager.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "PostEffect.h"
#include "Bloom.h"
#include "Outline.h"
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
	/// imgui�`��
	/// </summary>
	void ImguiDraw();

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
	std::unique_ptr<PostEffect> postEffect;
	//�|�X�g�G�t�F�N�g���H�p
	std::unique_ptr<Bloom> bloom;
	//�|�X�g�G�t�F�N�g���H�p
	std::unique_ptr<Outline> outline;
	//Fps�Œ�p�N���X�̃C���X�^���X
	std::unique_ptr<FrameRateKeep> fps = nullptr;
	//�����\���f�o�b�O�p
	wchar_t str[256] = {};
};