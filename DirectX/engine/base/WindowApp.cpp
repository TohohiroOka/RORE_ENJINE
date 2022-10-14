#include "WindowApp.h"
#include <imgui_impl_win32.h>

UINT WindowApp::windowWidth;
UINT WindowApp::windowHeight;
WNDCLASSEX WindowApp::winClass{};
HWND WindowApp::hwnd = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);

LRESULT WindowApp::WindowProcdure(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(_hwnd, _msg, _wparam, _lparam)) {
		return 1;
	}

	if (_msg == WM_DESTROY)
	{
		PostQuitMessage((0));
		return 0;
	}
	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}

void WindowApp::Initialize(int _windowWidth, int _windowHeight,const wchar_t* _gameName)
{
	windowWidth = _windowWidth;
	windowHeight = _windowHeight;

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.lpfnWndProc = (WNDPROC)WindowProcdure;// �E�B���h�E�v���V�[�W����ݒ�
	winClass.lpszClassName = _gameName;// �E�B���h�E�N���X��
	winClass.hInstance = GetModuleHandle(nullptr);// �E�B���h�E�n���h��
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);// �J�[�\���w��

	// �E�B���h�E�N���X��OS�ɓo�^
	RegisterClassEx(&winClass);
	// �E�B���h�E�T�C�Y{ X���W Y���W ���� �c�� }
	RECT wrc = { 0, 0, (LONG)windowWidth, (LONG)windowHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);// �����ŃT�C�Y�␳

	// �E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(winClass.lpszClassName,// �N���X��
		_gameName,// �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,// �W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,// �\��X���W�iOS�ɔC����j
		CW_USEDEFAULT,// �\��Y���W�iOS�ɔC����j
		wrc.right - wrc.left,// �E�B���h�E����
		wrc.bottom - wrc.top,// �E�B���h�E�c��
		nullptr,// �e�E�B���h�E�n���h��
		nullptr,// ���j���[�n���h��

		winClass.hInstance,// �Ăяo���A�v���P�[�V�����n���h��
		nullptr);// �I�v�V����

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);
}

bool WindowApp::Update()
{
	//���b�Z�[�W����
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//�A�v���P�[�V�������I��鎞��message��WM_QUIT�ɂȂ�
	if (msg.message == WM_QUIT)
	{
		return true;
	}
	else { return false; }
}

void WindowApp::Release()
{
	UnregisterClass(winClass.lpszClassName, winClass.hInstance);
}