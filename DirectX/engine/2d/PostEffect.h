#pragma once
#include "Sprite.h"

class PostEffect : public Sprite
{
public:

	enum class EffectTyep {
		normal,
	};

public://メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	static std::unique_ptr<PostEffect> Create();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

public://メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	PostEffect();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PostEffect();

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
	static void SetPipeline(const std::vector<GraphicsPipelineManager::DrawSet>& _pipeline) { PostEffect::pipeline = _pipeline; }

};