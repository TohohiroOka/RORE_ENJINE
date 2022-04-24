#pragma once
#include "Sprite.h"

class DebugText : public Sprite
{
public:
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	static const int maxCharCount = 256;	// �ő啶����
	static const int fontWidth = 9;			// �t�H���g�摜��1�������̉���
	static const int fontHeight = 18;		// �t�H���g�摜��1�������̏c��
	static const int fontLineCount = 14;	// �t�H���g�摜��1�s���̕�����
	static const int bufferSize = 256;	// �����t��������W�J�p�o�b�t�@�T�C�Y

private://�ÓI�����o�ϐ�

	// �X�v���C�g�f�[�^�̔z��
	static std::unique_ptr<Sprite> spriteDatas[maxCharCount];

public:// �ÓI�����o�֐�

	/// <summary>
	/// �C���X�^���X�̐���
	/// </summary>
	/// <returns>�C���X�^���X</returns>
	static DebugText* GetInstance();

public:// �����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="texnumber">�g���e�N�X�`���̔ԍ�</param>
	void Initialize(UINT texnumber);

	/// <summary>
	/// �ʒu�̃Z�b�g
	/// </summary>
	/// <param name="x">x��</param>
	/// <param name="y">y��</param>
	inline void SetPos(XMFLOAT2 pos) {this->pos = pos;}

	/// <summary>
	/// �傫���̃Z�b�g
	/// </summary>
	/// <param name="size"></param>
	inline void SetSize(float size) { this->size = size; }

	/// <summary>
	/// �o�͂��镶���̐ݒ�
	/// </summary>
	/// <param name="text">����</param>
	/// <param name="pos">���W</param>
	/// <param name="size">�傫��</param>
	void Print(const std::string& text, XMFLOAT2 pos, float size = 1.0f);

	/// <summary>
	/// ���l�̕����o��
	/// </summary>
	/// <param name="num">���l</param>
	/// <param name="pos">���W</param>
	/// <param name="size">�傫��</param>
	void PrintNum(const float& num, XMFLOAT2 pos, float size = 1.0f);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="len">������̒���</param>
	/// <param name="text">�o�͂��镶��</param>
	void NPrint(int len, const char* text);

	/// <summary>
	/// �S�Ă̕`��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void DrawAll(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �������
	/// </summary>
	static void Finalize();

private:
	DebugText() {};
	DebugText(const DebugText&) = delete;
	~DebugText() {};
	DebugText& operator=(const DebugText&) = delete;

private:
	// �X�v���C�g�f�[�^�z��̓Y�����ԍ�
	int spriteIndex = 0;

	XMFLOAT2 pos = {};
	float size = 1.0f;

	char buffer[bufferSize];
};