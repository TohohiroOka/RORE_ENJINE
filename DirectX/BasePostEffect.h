#pragma once
#include "Sprite.h"

class BasePostEffect : public Sprite
{
public:

	enum class EffectTyep {
		normal,
		bloom,
		outline,
		size,
	};

public://メンバ関数

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	static std::unique_ptr<BasePostEffect> Create();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

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
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// 描画コマンドの発行
	/// </summary>
	void Draw(const EffectTyep _drawMode = EffectTyep::normal);

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

public:
	static void SetPipeline(const std::vector<GraphicsPipelineManager::DrawSet>& _pipeline) { BasePostEffect::pipeline = _pipeline; }

};