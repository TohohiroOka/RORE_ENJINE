#pragma once
#include "BasePostEffect.h"

class Outline : public BasePostEffect
{
private:

	struct CONST_BUFFER_DATA {
		float outlineWidth;//アウトラインの太さ
		XMFLOAT4 pad;
	};

private:

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Outline();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Outline() {};

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	static std::unique_ptr<Outline> Create();

	/// <summary>
	/// 描画コマンドの発行
	/// </summary>
	void Draw(const Texture* _tex);

private:

	//アウトラインの色
	float outlineWidth;//アウトラインの太さ

public:
	void SetOutlineWidth(const float _outlineWidth) { outlineWidth = _outlineWidth; }

};