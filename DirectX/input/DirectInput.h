#pragma once
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800	//DirectInput�̃o�[�W�����w��
#include <dinput.h>
#include <DirectXMath.h>

class DirectInput final
{
public: //�G�C���A�X
	//namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //��
	//�}�E�X�{�^��
	enum MouseButton
	{
		MOUSE_LEFT,
		MOUSE_RIGHT,
		MOUSE_WHEEL
	};

private: //�V���O���g����
	//�R���X�g���N�^���B��
	DirectInput() = default;
	//�f�X�g���N�^���B��
	~DirectInput() = default;

public:
	//�R�s�[�R���X�g���N�^�𖳌���
	DirectInput(const DirectInput& input) = delete;
	//������Z�q�𖳌���
	void operator = (const DirectInput& input) = delete;

public: //�����o�֐�

	/// <summary>
	/// �C���X�^���X�擾
	/// </summary>
	/// <returns>����</returns>
	static DirectInput* GetInstance();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �L�[�̉������`�F�b�N
	/// </summary>
	/// <param name = "keyNumber">�L�[�ԍ�( DIK_0 ��)</param>
	/// <returns>������Ă��邩</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// �L�[�̃g���K�[���`�F�b�N
	/// </summary>
	/// <param name = "keyNumber">�L�[�ԍ�( DIK_0 ��)</param>
	/// <returns>�g���K�[��</returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// �L�[�𗣂������`�F�b�N
	/// </summary>
	/// <param name = "keyNumber">�L�[�ԍ�( DIK_0 ��)</param>
	/// <returns>��������</returns>
	bool ReleaseKey(BYTE keyNumber);

	/// <summary>
	/// �}�E�X�{�^���̉������`�F�b�N
	/// </summary>
	/// <param name = "mouseButton">�}�E�X�{�^��</param>
	/// <returns>������Ă��邩</returns>
	bool PushMouseButton(const int mouseButton);

	/// <summary>
	/// �}�E�X�{�^���̃g���K�[���`�F�b�N
	/// </summary>
	/// <param name = "mouseButton">�}�E�X�{�^��</param>
	/// <returns>�g���K�[��</returns>
	bool TriggerMouseButton(const int mouseButton);

	/// <summary>
	/// �}�E�X�{�^���𗣂������`�F�b�N
	/// </summary>
	/// <param name = "mouseButton">�}�E�X�{�^��</param>
	/// <returns>��������</returns>
	bool ReleaseMouseButton(const int mouseButton);

	/// <summary>
	/// �}�E�X�|�C���^�[�̍��W���擾
	/// </summary>
	/// <returns>�}�E�X�|�C���^�[�̍��W</returns>
	DirectX::XMFLOAT2 GetMousePoint();

	/// <summary>
	/// 1�t���[���̃}�E�X�̈ړ��ʂ̎擾
	/// </summary>
	/// <returns>1�t���[���̃}�E�X�̈ړ���</returns>
	DirectX::XMFLOAT2 GetMouseVelocity();

	/// <summary>
	/// 1�t���[���̃}�E�X�z�C�[���̈ړ��ʂ̎擾
	/// </summary>
	/// <returns>1�t���[���̃}�E�X�z�C�[���̈ړ���</returns>
	float GetMouseWheelVelocity();

private: //�����o�ϐ�
	//DirectInput�̃C���X�^���X����
	ComPtr<IDirectInput8> dinput;
	//�L�[�{�[�h�̃f�o�C�X
	ComPtr<IDirectInputDevice8> devkeyboard;
	//�S�L�[�̏��
	BYTE key[256] = {};
	//�O��̑S�L�[�̏��
	BYTE keyPre[256] = {};
	//�}�E�X�̃f�o�C�X
	ComPtr<IDirectInputDevice8> devmouse;
	//�}�E�X�̏��
	DIMOUSESTATE mouse;
	//�O��̃}�E�X�̏��
	DIMOUSESTATE mousePre;
	//�}�E�X�|�C���^�[
	POINT mousePoint;
};