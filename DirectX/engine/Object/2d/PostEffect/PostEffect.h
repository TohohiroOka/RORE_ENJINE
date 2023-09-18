#pragma once
#include "Object/2d/Sprite.h"

class PostEffect : public Sprite
{
public:

	enum class TexType {
		normal,
		bloom,
		outline,
		size,
	};

private:

	/// <summary>
	/// 定数バッファ用データ構造体
	/// </summary>
	struct CONST_BUFFER_DATA
	{
		float outline;
	};

public://メンバ関数

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
	/// 描画
	/// </summary>
	void Draw(const std::vector<Texture*> _tex);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDrawScene();
	
	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDrawScene();

	Texture* GetTex(TexType _type) {
		return texture[int(_type)].get();
	}

private://静的メンバ変数

	//画面クリアカラー
	static const float clearColor[4];
	//パイプライン情報
	static std::vector<GraphicsPipelineManager::DrawSet> pipeline;

private://メンバ変数

	//テクスチャ情報
	std::array<std::unique_ptr<Texture>, int(TexType::size)> texture;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff;


public:
	static void SetPipeline(const std::vector<GraphicsPipelineManager::DrawSet>& _pipeline) { PostEffect::pipeline = _pipeline; }

};