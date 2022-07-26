#pragma once
#include "InterfaceObject3d.h"

class BaseCollider;
class Camera;
class LightGroup;

class PrimitiveObject3D : public InterfaceObject3d
{
public://静的メンバ関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PrimitiveObject3D();

	/// <summary>
	/// 描画前処理
	/// </summary>
	static void PreDraw();

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	/// <param name="pipeline"></param>
	static void SetPipeline(GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline) { PrimitiveObject3D::pipeline = pipeline; }

public://メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// //描画
	/// </summary>
	void Draw() override;

private://静的メンバ変数

	//パイプライン
	static GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline;

protected://メンバ変数

	// 頂点配列
	std::vector<XMFLOAT3> vertices;
	//インデックス配列
	std::vector<unsigned short> indices;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};

public:

	/// <summary>
	/// 頂点のセット
	/// </summary>
	/// <param name="model">頂点</param>
	void SetVertex(XMFLOAT3 vertex) { this->vertices.push_back(vertex); }
};