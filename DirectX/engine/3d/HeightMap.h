#pragma once
#include "InterfaceObject3d.h"
#include "TerrainModel.h"

class BaseCollider;
class Camera;
class LightGroup;

class HeightMap : public InterfaceObject3d
{
private:// サブクラス

	// 定数バッファ用データ構造体B0
	struct CONST_BUFFER_DATA_B0
	{
		XMFLOAT4 baseColor;//ベースカラー
		XMMATRIX viewproj; // ビュープロジェクション行列
		XMMATRIX world; // ワールド行列
		XMFLOAT3 cameraPos; // カメラ座標（ワールド座標）
		unsigned int isSkinning;//スキニングを行うか
		unsigned int isBloom;//ブルームの有無
		unsigned int isToon;//トゥーンの有無
		unsigned int isOutline;//アウトラインの有無
		unsigned int isLight;//ライティングの有無
	};

public://メンバ関数

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_model">モデル</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<HeightMap> Create(TerrainModel* _model);

	/// <summary>
	/// 描画前処理
	/// </summary>
	static void PreDraw();

private://メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// モデルセット
	/// </summary>
	/// <param name="_model">モデル</param>
	void SetModel(TerrainModel* _model) {
		model = _model;
	}

public:

	HeightMap() {};
	~HeightMap() {};

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// パイプラインのセット
	/// </summary>
	/// <param name="_pipeline">パイプライン</param>
	static void SetPipeline(const GraphicsPipelineManager::GRAPHICS_PIPELINE& _pipeline) { HeightMap::pipeline = _pipeline; }
	
	/// <summary>
	/// モデルの変換
	/// </summary>
	/// <param name="_beforeModel">変更前</param>
	/// <param name="afterModel">変更後</param>
	/// <param name="_ratio">変換割合</param>
	void SetChangeDrawModel(TerrainModel* _beforeModel, TerrainModel* afterModel, const float _ratio = 1.0f);

	/// <summary>
	/// モデルの変換
	/// </summary>
	/// <param name="_beforeModel">変更前</param>
	/// <param name="afterModel">変更後</param>
	/// <param name="_ratio">変換割合</param>
	void SetChangeHitModel(TerrainModel* _beforeModel, TerrainModel* afterModel, const float _ratio = 1.0f);

private:

	//パイプライン
	static GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline;

private:

	//定数バッファ
	ComPtr<ID3D12Resource> constBufferOData;
	// モデル
	TerrainModel* model = nullptr;

public:

	Model* GetModel() { return model->GetModel(); }
	std::vector<Mesh::VERTEX>& GetHitVertices() { return model->GetHitVertices(); }
	std::vector<unsigned long>& GetHitIndices() { return model->GetHitIndices(); }
};