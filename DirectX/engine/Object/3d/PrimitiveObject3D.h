#pragma once
#include "Base3D.h"

class BaseCollider;
class Camera;
class LightGroup;

class PrimitiveObject3D : public Base3D
{
private://構造体宣言

	//定数バッファの構造体
	struct CONST_BUFFER_DATA {
		XMFLOAT4 color;//色
		XMMATRIX matWorld;//3D変換行列
		XMMATRIX viewproj;//3D変換行列
	};

public:

	/// <summary>
	/// 生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<PrimitiveObject3D> Create();

public://メンバ関数


	PrimitiveObject3D() {};
	~PrimitiveObject3D();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;


	/// <summary>
	/// //描画
	/// </summary>
	void Draw(const DrawMode _drawMode = DrawMode::alpha);

	/// <summary>
	/// 頂点情報の再設定
	/// </summary>
	void VertexInit();

	/// <summary>
	/// 
	/// </summary>
	void SetWorld(const XMMATRIX& _world) {
		matWorld =_world;
	}

protected://メンバ変数

	//パイプライン情報
	static std::vector<GraphicsPipelineManager::DrawSet> pipeline;

	// 頂点配列
	std::vector<XMFLOAT3> vertices;
	//インデックス配列
	std::vector<unsigned long> indices;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB0;
	//ベースカラー
	XMFLOAT4 baseColor = { 1,1,1,1 };

public:

	/// <summary>
	/// 頂点のセット
	/// </summary>
	/// <param name="_vertex">頂点</param>
	void SetVertex(const XMFLOAT3& _vertex) { this->vertices.push_back(_vertex); }

	/// <summary>
	/// 頂点のリセット
	/// </summary>
	void ResetVertex() { 
		this->vertices.clear();
		this->vertices.resize(0);
	}

	/// <summary>
	/// 頂点の数
	/// </summary>
	int VertexNum() { return int(vertices.size()); }

	static void SetPipeline(const std::vector<GraphicsPipelineManager::DrawSet>& _pipeline) { PrimitiveObject3D::pipeline = _pipeline; }
	void SetColor(const XMFLOAT4& _color) { baseColor = _color; }
};