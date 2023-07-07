#pragma once
#include "BasePostEffect.h"

class Bloom :public BasePostEffect
{
private:

	struct CONST_BUFFER_DATA {
		float strength;//ブルームの強さ
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
	Bloom();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Bloom() {};

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	static std::unique_ptr<Bloom> Create();

	/// <summary>
	/// 描画コマンドの発行
	/// </summary>
	void Draw(const Texture* _tex);

private:

	//ブルームの強さ
	float strength;

public:
	void SetStrength(const float _strength) { strength = _strength; }

};