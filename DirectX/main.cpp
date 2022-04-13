#include "MainEngine.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// �E�B���h�E�T�C�Y
	const int window_width = 1280;// ����
	const int window_height = 720;// �c��

	//�Q�[����
	const wchar_t* gameName = L"NAVE";

	//�S�̂̏�����
	MainEngine* engine = nullptr;
	engine = new MainEngine();
	engine->Initialize(gameName, window_width, window_height);

	while (true)
	{
		if (engine->Update() == true) { break; }
		engine->Draw();
		engine->frameRateKeep();
	}

	//�o�^����
	delete engine;

	return 0;
}