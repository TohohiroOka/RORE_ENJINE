#pragma once
#include "BasePostEffect.h"
class Fog : public BasePostEffect
{
private:

	struct CONST_BUFFER_DATA {
		float strength;//フォグの強さ
		XMFLOAT3 color;//フォグの色
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
	Fog();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Fog() {};

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	static std::unique_ptr<Fog> Create();

	/// <summary>
	/// 描画コマンドの発行
	/// </summary>
	void Draw(const Texture* _tex);

private:

	//フォグの強さ
	float strength;
	//フォグの色
	XMFLOAT3 color;

public:
	void SetStrength(const float _strength) { strength = _strength; }
	void SetColor(const XMFLOAT3& _color) { color = _color; }
};

