#pragma once
#include "InterfaceObject3d.h"

class BaseCollider;
class Camera;
class LightGroup;

class Object3d : public InterfaceObject3d
{
public: // 静的メンバ関数

	/// <summary>
	/// 描画前処理
	/// </summary>
	static void PreDraw();

	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<Object3d> Create(Model* model = nullptr);

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	/// <param name="pipeline"></param>
	static void SetPipeline(GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline) { Object3d::pipeline = pipeline; }

private: // 静的メンバ変数

	//パイプライン
	static GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline;

public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	void Initialize() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

protected: // メンバ変数

	// 名前
	const char* name = nullptr;
	// モデル
	Model* model = nullptr;

public:

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="model">モデル</param>
	void SetModel(Model* model) { this->model = model; }

	/// <summary>
	/// モデルを取得
	/// </summary>
	/// <returns>モデル</returns>
	inline Model* GetModel() { return model; }
};