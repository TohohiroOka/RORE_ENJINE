#pragma once
#include "Sprite.h"

class BasePostEffect :public Sprite
{
private:

	struct CONST_BUFFER_DATA_BLOOM {
		float strength;//ブルームの強さ
		XMFLOAT4 pad;
	};

	struct CONST_BUFFER_DATA_OUTLINE {
		float outlineWidth;//アウトラインの太さ
		XMFLOAT4 pad;
	};

public:

	enum class EffectTyep {
		bloom,
		outline,
		size,
	};

private:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer(const EffectTyep _type);

public://メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BasePostEffect();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BasePostEffect();

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	static std::unique_ptr<BasePostEffect> Create(const EffectTyep _type);

	/// <summary>
	/// 描画コマンドの発行
	/// </summary>
	void Draw(const Texture* _tex);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDrawScene();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDrawScene();

	Texture* GetTex() {
		return texture.get();
	}

private://静的メンバ変数

	//画面クリアカラー
	static const float clearColor[4];
	//パイプライン情報
	static std::vector<GraphicsPipelineManager::DrawSet> pipeline;

private://メンバ変数

	//テクスチャ情報
	std::unique_ptr<Texture> texture;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//深度バッファ
	ComPtr<ID3D12Resource> depthBuffer;
	//エフェクト種類
	EffectTyep type;

	//ブルームの強さ
	float strength;
	//アウトラインの太さ
	float outlineWidth;

public:
	static void SetPipeline(const std::vector<GraphicsPipelineManager::DrawSet>& _pipeline) { BasePostEffect::pipeline = _pipeline; }
	void SetStrength(const float _strength) { strength = _strength; }
	void SetOutlineWidth(const float _outlineWidth) { outlineWidth = _outlineWidth; }

};