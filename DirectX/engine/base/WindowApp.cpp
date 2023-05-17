#include "WindowApp.h"
#include <imgui_impl_win32.h>

UINT WindowApp::windowWidth;
UINT WindowApp::windowHeight;
WNDCLASSEX WindowApp::winClass{};
HWND WindowApp::hwnd = nullptr;
bool WindowApp::isWindowResize;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

LRESULT WindowApp::WindowProcdure(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	//if (_msg == WM_SIZE) {
	//	//ウィンドウサイズ取得
	//	RECT window_rect;
	//	GetWindowRect(_hwnd, &window_rect);

	//	////クライアント領域取得
	//	//RECT client_rect;
	//	//GetClientRect(_hwnd, &client_rect);

	//	// フレームサイズ算出
	//	int frame_size_x = (window_rect.right - window_rect.left);// - (client_rect.right - client_rect.left);
	//	int frame_size_y = (window_rect.bottom - window_rect.top);// - (client_rect.bottom - client_rect.top);

	//	window_rect = { 0,0,frame_size_x ,frame_size_y };

	//	//// リサイズ用サイズの算出
	//	//int resize_width = frame_size_x + windowWidth;
	//	//int resize_height = frame_size_y + windowHeight;

	//	windowWidth = frame_size_x;
	//	windowHeight = frame_size_y;

	//	AdjustWindowRect(&window_rect, WS_VSCROLL, false);// 自動でサイズ補正

	//	//SetWindowPos(
	//	//	// ウィンドウハンドル
	//	//	_hwnd,
	//	//	// 配置順序のハンドル(NULLでよし)
	//	//	NULL,
	//	//	// 表示座標X
	//	//	CW_USEDEFAULT,
	//	//	// 表示座標Y
	//	//	CW_USEDEFAULT,
	//	//	// リサイズ横幅
	//	//	frame_size_x,
	//	//	// リサイズ縦幅
	//	//	frame_size_y,
	//	//	// SWP_NOMOVE => 位置変更なし
	//	//	SWP_NOMOVE);

	//	// ウィンドウ表示
	//	//ShowWindow(hwnd, SW_SHOW);

	//	isWindowResize = true;
	//}

	if (_msg == WM_DESTROY)
	{
		PostQuitMessage((0));
		return 0;
	}
	ImGui_ImplWin32_WndProcHandler(_hwnd, _msg, _wparam, _lparam);

	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}

void WindowApp::Initialize(int _windowWidth, int _windowHeight,const wchar_t* _gameName)
{
	windowWidth = _windowWidth;
	windowHeight = _windowHeight;

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.lpfnWndProc = (WNDPROC)WindowProcdure;// ウィンドウプロシージャを設定
	winClass.lpszClassName = _gameName;// ウィンドウクラス名
	winClass.hInstance = GetModuleHandle(nullptr);// ウィンドウハンドル
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);// カーソル指定

	// ウィンドウクラスをOSに登録
	RegisterClassEx(&winClass);
	// ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
	RECT wrc = { 0, 0, (LONG)windowWidth, (LONG)windowHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);// 自動でサイズ補正

	// ウィンドウオブジェクトの生成
	hwnd = CreateWindow(winClass.lpszClassName,// クラス名
		_gameName,// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,// 標準的なウィンドウスタイル
		CW_USEDEFAULT,// 表示X座標（OSに任せる）
		CW_USEDEFAULT,// 表示Y座標（OSに任せる）
		wrc.right - wrc.left,// ウィンドウ横幅
		wrc.bottom - wrc.top,// ウィンドウ縦幅
		nullptr,// 親ウィンドウハンドル
		nullptr,// メニューハンドル

		winClass.hInstance,// 呼び出しアプリケーションハンドル
		nullptr);// オプション

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);
}

bool WindowApp::Update()
{
	isWindowResize = false;
	//メッセージ処理
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//アプリケーションが終わる時にmessageがWM_QUITになる
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