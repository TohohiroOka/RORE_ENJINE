#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include "ShaderManager.h"

class GraphicsPipelineManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://メンバenum

	enum OBJECT_KINDS
	{
		NONE,
		OBJ,
		SPRITE,
		FBX,
		DRAW_LINE_2D,
		DRAW_LINE_3D,
		PARTICLE,
		NORMAL_MAP,
		POST_EFFECT,
		CUBE_BOX,
	};

private://メンバ関数

	/// <summary>
	/// グラフィックスパイプラインの基盤の生成
	/// </summary>
	static void CreateDefaultGpipeline();

	/// <summary>
	/// パイプラインデスクの生成
	/// </summary>
	/// <param name="inputLayout">頂点レイアウト</param>
	/// <param name="inputLayoutSize">頂点レイアウトのサイズ</param>
	/// <returns>パイプラインデスク</returns>
	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatepelineDesc(
		const D3D12_INPUT_ELEMENT_DESC* inputLayout, const UINT inputLayoutSize);

	/// <summary>
	/// ルートシグネチャの生成
	/// </summary>
	void CreateRootSignature();

public://メンバ関数

	GraphicsPipelineManager() {};
	~GraphicsPipelineManager();

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <param name="className">呼び出しクラスの名前</param>
	/// <param name="objectKind">オブジェクトの種類</param>
	/// <param name="inputLayout">頂点レイアウト</param>
	/// <param name="inputLayoutSize">頂点レイアウトのサイズ</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<GraphicsPipelineManager> Create(const std::string name, const unsigned char objectKind,
		const D3D12_INPUT_ELEMENT_DESC* inputLayout, const UINT inputLayoutSize);

	/// <summary>
	/// パイプラインの生成
	/// enum OBJECT_KINDSの中から一を引数とする
	/// </summary>
	/// <param name="className">呼び出しクラスの名前</param>
	/// <param name="objectKind">オブジェクトの種類</param>
	/// <param name="inputLayout">頂点レイアウト</param>
	/// <param name="inputLayoutSize">頂点レイアウトのサイズ</param>
	void CreatePipeline(const std::string name, const unsigned char objectKind,
		const D3D12_INPUT_ELEMENT_DESC* inputLayout, const UINT inputLayoutSize);

	/// <summary>
	/// 解放処理
	/// </summary>
	static void Finalize();

private://メンバ変数

	// デバイス
	static ID3D12Device* device;
	//シェーダー
	static std::unique_ptr<ShaderManager> shaderManager;
	//生成オブジェクトの種類
	unsigned char objectKind = OBJECT_KINDS::NONE;
	//グラフィックスパイプラインの基盤
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC defaultGpipeline;
	//ブレンド設定の基盤
	static D3D12_RENDER_TARGET_BLEND_DESC blenddesc;

public://メンバ変数

	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	// パイプラインステートオブジェクト
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
};
