#pragma once
#include "Sprite.h"

class PostEffect : public Sprite
{
private://静的メンバ関数

	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	static void CreateGraphicsPipeline();

public: // サブクラス

	// 定数バッファ用データ構造体B0
	struct ConstBufferData
	{
		XMFLOAT4 outlineColor;//アウトラインの色
		float outlineWidth;//アウトラインの太さ
		unsigned int isFog;//フォグの有無
		unsigned int isGaussianBlur;//ガウスブラーの有無
		float BlurStrength;//ブラー強度
	};

	enum TEX_TYPE
	{
		NORMAL,
		BLOOM,
		OUTLINE,
		DEPTH
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
	/// <param name="cmdList">コマンドリスト</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

private://静的メンバ変数
	
	//通常テクスチャ数
	static const int texBuffNum = 4;
	static const int normalTexNum = 3;
	//画面クリアカラー
	static const float clearColor[2][4];
	//パイプライン
	static std::unique_ptr<GraphicsPipelineManager> pipeline;
	//フォグ
	static bool isFog;

private://メンバ変数

	//テクスチャ情報
	std::array<std::unique_ptr<Texture>, texBuffNum> texture;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;

public:

	static bool GetFog() { return PostEffect::isFog; }
	static void SetFog(bool isFog) { PostEffect::isFog = isFog; }
};