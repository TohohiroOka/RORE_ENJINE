#pragma once
#include <Windows.h>
#include "Singleton.h"

class WindowApp : public Singleton< WindowApp >
{
	friend Singleton< WindowApp >;

private:

	WindowApp() {};
	~WindowApp() {};

public:
	//�E�B���h�E�v���\�W���̐���
	static LRESULT WindowProcdure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	MSG msg = {};

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="windowWidth">��ʕ�</param>
	/// <param name="windowHeight">��ʍ���</param>
	/// <param name="gameName">�^�C�g���o�[��</param>
	void Initialize(const int windowWidth, const int windowHeight, const wchar_t* gameName);

	/// <summary>
	/// �X�V
	/// </summary>
	/// <returns>�A�v���P�[�V�������I�����邩�ۂ�</returns>
	bool Update();

	/// <summary>
	/// �o�^����
	/// </summary>
	void Release();

	/// <summary>
	/// �E�B���h�E�I�u�W�F�N�g�̎擾
	/// </summary>
	/// <returns>hwnd</returns>
	static HWND GetHwnd() { return hwnd; }

	/// <summary>
	/// �E�B���h�E�N���X�ݒ�̎擾
	/// </summary>
	/// <returns>hInstance</returns>
	static HINSTANCE GetWinInstance() { return winClass.hInstance; }

	/// <summary>
	/// �����̎擾
	/// </summary>
	/// <returns>windowWidth</returns>
	static UINT GetWindowWidth() { return windowWidth; }

	/// <summary>
	/// �c���̎擾
	/// </summary>
	/// <returns>windowHeight</returns>
	static UINT GetWindowHeight() { return windowHeight; }

private:

	//����
	static UINT windowWidth;
	//����
	static UINT windowHeight;
	//�E�B���h�E�N���X�̐ݒ�
	static WNDCLASSEX winClass;
	//�E�B���h�E�I�u�W�F�N�g
	static HWND hwnd;
};

