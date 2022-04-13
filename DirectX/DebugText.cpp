#include "DebugText.h"

Sprite* DebugText::spriteDatas[maxCharCount] = {};

DebugText::DebugText()
{
}

DebugText::~DebugText()
{
}

void DebugText::AllDelete()
{
	for (int i = 0; i < _countof(spriteDatas); i++) {
		delete spriteDatas[i];
	}
}

DebugText* DebugText::GetInstance()
{
	static DebugText instance;
	return &instance;
}

void DebugText::Initialize(UINT texnumber)
{
	// �S�ẴX�v���C�g�f�[�^�ɂ���
	for (int i = 0; i < _countof(spriteDatas); i++)
	{
		// �X�v���C�g�𐶐�����
		spriteDatas[i] = Sprite::Create(texnumber, { 0, 0 });
	}
}

void DebugText::Print(const std::string& text, float x, float y, float size)
{
	SetPos(x, y);
	SetSize(size);

	NPrint((UINT)text.size(), text.c_str());
}

void DebugText::NPrint(int len, const char* text)
{
	// �S�Ă̕����ɂ���
	for (int i = 0; i < len; i++)
	{
		// �ő啶��������
		if (spriteIndex >= maxCharCount) {
			break;
		}

		// 1�������o��(��ASCII�R�[�h�ł������藧���Ȃ�)
		const unsigned char& character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f) {
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

		// �������P�i�߂�
		spriteIndex++;
	}
}

void DebugText::DrawAll(ID3D12GraphicsCommandList* cmdList)
{
	// �S�Ă̕����̃X�v���C�g�ɂ���
	for (int i = 0; i < spriteIndex; i++)
	{
		// �X�v���C�g�`��
		spriteDatas[i]->Draw();
	}

	spriteIndex = 0;
}