#pragma once

#include "Sprite.h"
#include <Windows.h>
#include <string>

/// <summary>
/// デバッグ用文字表示
/// </summary>
class DebugText
{
public:
	// デバッグテキスト用のテクスチャ番号を指定
	static const int maxCharCount = 256;	// 最大文字数
	static const int fontWidth = 9;			// フォント画像内1文字分の横幅
	static const int fontHeight = 18;		// フォント画像内1文字分の縦幅
	static const int fontLineCount = 14;	// フォント画像内1行分の文字数
	static const int bufferSize = 256;	// 書式付き文字列展開用バッファサイズ

private://静的メンバ変数

	// スプライトデータの配列
	static Sprite* spriteDatas[maxCharCount];


public:// 静的メンバ関数

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static DebugText* GetInstance();

	/// <summary>
	/// 解放処理
	/// </summary>
	static void AllDelete();

public:// メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="texnumber">使うテクスチャの番号</param>
	void Initialize(UINT texnumber);

	/// <summary>
	/// 位置のセット
	/// </summary>
	/// <param name="x">x軸</param>
	/// <param name="y">y軸</param>
	inline void SetPos(float x, float y) {
		posX = x;
		posY = y;
	}

	inline void SetSize(float size) { this->size = size; }

	/// <summary>
	/// 出力する文字の設定
	/// </summary>
	/// <param name="text">文字</param>
	/// <param name="x">x軸の位置</param>
	/// <param name="y">y軸の位置</param>
	/// <param name="size">大きさ</param>
	void Print(const std::string& text, float x, float y, float size = 1.0f);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="len">文字列の長さ</param>
	/// <param name="text">出力する文字</param>
	void NPrint(int len, const char* text);

	/// <summary>
	/// 全ての描画
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void DrawAll(ID3D12GraphicsCommandList* cmdList);

private:
	DebugText();
	DebugText(const DebugText&) = delete;
	~DebugText();
	DebugText& operator=(const DebugText&) = delete;

private:
	// スプライトデータ配列の添え字番号
	int spriteIndex = 0;

	float posX = 0.0f;
	float posY = 0.0f;
	float size = 1.0f;

	char buffer[bufferSize];
};