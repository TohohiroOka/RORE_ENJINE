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
	
	//テクスチャバッファの数
	static const int texBuffNum = 3;
	//画面クリアカラー
	static const float clearColor[texBuffNum][4];
	//テクスチャバッファ
	static ComPtr<ID3D12Resource> texBuff[texBuffNum];
	//パイプライン
	static std::unique_ptr<GraphicsPipelineManager> pipeline;

private://メンバ変数

	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> depthBuff;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
};