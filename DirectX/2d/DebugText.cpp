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
	// 全てのスプライトデータについて
	for (int i = 0; i < _countof(spriteDatas); i++)
	{
		// スプライトを生成する
		spriteDatas[i] = Sprite::Create("debugfont");
		spriteDatas[i]->SetAnchorpoint({ 0, 0 });
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
	// 全ての文字について
	for (int i = 0; i < len; i++)
	{
		// 最大文字数超過
		if (spriteIndex >= maxCharCount) {
			break;
		}

		// 1文字取り出す(※ASCIIコードでしか成り立たない)
		const unsigned char& character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f) {
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		// 座標計算
		spriteDatas[spriteIndex]->SetPosition({ this->posX + fontWidth * this->size * i, this->posY });
		spriteDatas[spriteIndex]->SetTexLeftTop({ (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight });
		spriteDatas[spriteIndex]->SetTexSize({ (float)fontWidth, (float)fontHeight });
		spriteDatas[spriteIndex]->SetSize({ fontWidth * this->size, fontHeight * this->size });
		//更新
		spriteDatas[spriteIndex]->Update();

		spriteDatas[spriteIndex]->SetColor({ red,green,blue,1 });

		// 文字を１つ進める
		spriteIndex++;
	}
}

void DebugText::DrawAll(ID3D12GraphicsCommandList* cmdList)
{
	// 全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++)
	{
		// スプライト描画
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