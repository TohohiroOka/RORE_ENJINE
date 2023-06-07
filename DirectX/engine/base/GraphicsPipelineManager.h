#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include "ShaderManager.h"
#include <unordered_map>

class GraphicsPipelineManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://メンバenum

	struct GRAPHICS_PIPELINE
	{
		// ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootSignature = nullptr;
		// パイプラインステートオブジェクト
		ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	};

	struct PEPELINE {
		//shader名
		std::string name;
		//使うshaderの種類
		std::vector<std::string> shaderType;
		//layout設定
		std::vector<std::string> inputLayoutType;
		//レンダーターゲット数
		int rtvNum;
		//ブレンドモード
		std::string blendMode;
		//3D/2D/Particle
		std::string drawMode;
		//描画方法
		std::string drawType;
		//画像数
		int textureNum;
		//定数バッファ数
		int rootparams;
	};

	enum class DrawType {
		POINT,
		LINE_LIST,
		LINE_STRIP,
		TRIANGLE_LIST,
		TRIANGLE_STRIP,
	};

	struct DrawSet {
		std::string pipeName;
		DrawType drawType;
		int constBuffNum;
		int texNum;
	};

private://メンバ関数

	/// <summary>
	/// ブレンドモードの設定
	/// </summary>
	/// <param name="_mode">ブレンドの種類</param>
	/// <returns>ブレンド設定</returns>
	D3D12_RENDER_TARGET_BLEND_DESC CreateBlendDesc(const std::string& _mode);

	/// <summary>
	/// パイプラインデスクの生成
	/// </summary>
	/// <param name="_pepeline">パイプライン設定</param>
	/// <returns>パイプラインデスク</returns>
	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatepelineDesc(const PEPELINE& _pepeline);

	/// <summary>
	/// ルートシグネチャの生成
	/// </summary>
	/// <param name="_pepelineName">パイプライン名</param>
	/// <param name="_pepeline">パイプライン設定</param>
	void CreateRootSignature(const std::string& _pepelineName, const PEPELINE& _pepeline);

public://メンバ関数

	GraphicsPipelineManager();
	~GraphicsPipelineManager();

	/// <summary>
	/// デバイスのセット
	/// </summary>
	/// <param name="_device">デバイス</param>
	static void SetDevice(ID3D12Device* _device) { GraphicsPipelineManager::device = _device; }

	/// <summary>
	/// パイプラインの生成
	/// enum OBJECT_KINDSの中から一を引数とする
	/// </summary>
	/// <param name="_name">パイプライン名</param>
	/// <param name="_pepeline">パイプライン設定</param>
	/// <returns>パイプライン情報</returns>
	void CreatePipeline(const std::string& _name, const PEPELINE& _pepeline, GraphicsPipelineManager::DrawSet& _drawSet);

	/// <summary>
	/// 描画前処理
	/// </summary>
	static void PreDraw(ID3D12GraphicsCommandList* _cmdList, const DrawSet& _drawSet);

	/// <summary>
	/// フレーム初期化
	/// </summary>
	static void ResetDrawSet();

private://静的メンバ変数

	// デバイス
	static ID3D12Device* device;
	//パイプライン保存配列
	static std::unordered_map<std::string, GRAPHICS_PIPELINE> graphicsPipeline;
	//ひとつ前のパイプライン設定
	static DrawSet oldDraw;

private://メンバ変数

	//シェーダー
	std::unique_ptr<ShaderManager> shaderManager;
};