#include "DebugText.h"
#include <string>

std::unique_ptr<Sprite> DebugText::spriteDatas[maxCharCount] = {};

DebugText* DebugText::GetInstance()
{
	static DebugText instance;
	return &instance;
}

void DebugText::Initialize()
{
	// �S�ẴX�v���C�g�f�[�^�ɂ���
	for (int i = 0; i < _countof(spriteDatas); i++)
	{
		// �X�v���C�g�𐶐�����
		spriteDatas[i] = Sprite::Create("debugfont");
		spriteDatas[i]->SetAnchorpoint({ 0, 0 });
	}
}

void DebugText::Print(const std::string& _text, float _x, float _y, float _size)
{
	SetPos(_x, _y);
	SetSize(_size);

	NPrint((UINT)_text.size(), _text.c_str());
}

void DebugText::NPrint(int _len, const char* _text)
{
	// �S�Ă̕����ɂ���
	for (int i = 0; i < _len; i++)
	{
		// �ő啶��������
		if (spriteIndex >= maxCharCount) {
			break;
		}

		// 1�������o��(��ASCII�R�[�h�ł������藧���Ȃ�)
		const unsigned char& beeline = _text[i];

		int fontIndex = beeline - 32;
		if (beeline >= 0x7f) {
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		// ���W�v�Z
		spriteDatas[spriteIndex]->SetPosition({ this->posX + fontWidth * this->size * i, this->posY });
		spriteDatas[spriteIndex]->SetTexLeftTop({ (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight });
		spriteDatas[spriteIndex]->SetTexSize({ (float)fontWidth, (float)fontHeight });
		spriteDatas[spriteIndex]->SetSize({ fontWidth * this->size, fontHeight * this->size });
		//�X�V
		spriteDatas[spriteIndex]->Update();

		spriteDatas[spriteIndex]->SetColor({ red,green,blue,1 });

		// �������P�i�߂�
		spriteIndex++;
	}
}

void DebugText::DrawAll()
{
	// �S�Ă̕����̃X�v���C�g�ɂ���
	for (int i = 0; i < spriteIndex; i++)
	{
		// �X�v���C�g�`��
		spriteDatas[i]->Draw();
	}

	spriteIndex = 0;
}

void DebugText::Finalize()
{
	for (int i = 0; i < maxCharCount; i++)
	{
		spriteDatas[i].reset();
	}
}