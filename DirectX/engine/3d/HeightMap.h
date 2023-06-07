#pragma once
#include "Base3D.h"
#include "TerrainModel.h"

class Camera;
class LightGroup;

class HeightMap : public Base3D
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

private://メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

public:

	HeightMap() {};
	~HeightMap();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const DrawMode _drawMode = DrawMode::alpha);

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

	//パイプライン情報
	static std::vector<GraphicsPipelineManager::DrawSet> pipeline;

	// モデル
	TerrainModel* model = nullptr;

	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB0;
	//ベースカラー
	XMFLOAT4 baseColor = { 1,1,1,1 };
	//ブルームの有無
	bool isBloom = false;
	//トゥーンの有無
	bool isToon = false;
	//アウトラインの有無
	bool isOutline = false;
	//スキニング
	bool isSkinning = false;
	//ライティング
	bool isLight = true;

public:

	static void SetPipeline(const std::vector<GraphicsPipelineManager::DrawSet>& _pipeline) { HeightMap::pipeline = _pipeline; }
	void SetModel(TerrainModel* _model) { model = _model; }
	Model* GetModel() { return model->GetModel(); }
	std::vector<Mesh::VERTEX>& GetHitVertices() { return model->GetHitVertices(); }
	std::vector<unsigned long>& GetHitIndices() { return model->GetHitIndices(); }
};