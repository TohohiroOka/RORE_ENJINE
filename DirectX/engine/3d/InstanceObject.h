#pragma once
#include "Base3D.h"

class Camera;
class LightGroup;

class InstanceObject : public Base3D
{
private:
	//頂点データ3D
	struct VERTEX {
		XMFLOAT3 pos;
		XMFLOAT3 uv;
		XMFLOAT3 normal;
	};

	//一度に描画できる個数
	static const int draw_max_num = 512;

	struct OBJECT_INFO
	{
		XMFLOAT4 baseColor[draw_max_num];//ベースカラー
		XMMATRIX matWorld[draw_max_num];//world行列
	};

	// 定数バッファ用データ構造体B0
	struct CONST_BUFFER_DATA_B0
	{
		XMMATRIX viewproj; // ビュープロジェクション行列
		XMFLOAT3 cameraPos; // カメラ座標（ワールド座標）
		unsigned int isBloom;//ブルームの有無
		unsigned int isToon;//トゥーンの有無
		unsigned int isOutline;//アウトラインの有無
		unsigned int isLight;//ライティングの有無
	};

public://メンバ関数

	/// <summary>
	/// 
	/// </summary>
	/// <param name="_model">モデル</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<InstanceObject> Create(Model* _model);

private://メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_model">モデル</param>
	void Initialize(Model* _model);

public:

	InstanceObject() {};
	~InstanceObject();

	/// <summary>
	/// 描画セット
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_scale">大きさ</param>
	/// <param name="_rotation">回転角</param>
	/// <param name="_color">色</param>
	void DrawInstance(const XMFLOAT3& _pos, const XMFLOAT3& _scale,
		const XMFLOAT3& _rotation, const XMFLOAT4& _color);

	/// <summary>
	/// 描画セット
	/// </summary>
	/// <param name="_worldMat">ワールド行列</param>
	/// <param name="_color">色</param>
	void DrawInstance(const XMMATRIX& _worldMat, const XMFLOAT4& _color);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const DrawMode _drawMode = DrawMode::alpha);

	/// <summary>
	/// インスタンシング描画最大描画数になっていないかのチェック
	/// </summary>
	/// <returns></returns>
	bool GetInstanceDrawCheck() { return instanceDrawNum < draw_max_num; }

	/// <summary>
	/// インスタンシング描画個数
	/// </summary>
	/// <returns></returns>
	int GetInstanceDrawNum() { return instanceDrawNum; }

	/// <summary>
	/// 
	/// </summary>
	void FrameReset();

private:

	//パイプライン情報
	static std::vector<GraphicsPipelineManager::DrawSet> pipeline;

	//モデル
	Model* model;
	//オブジェクト情報配列
	OBJECT_INFO objInform;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB0;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB1;
	//ブルームの有無
	bool isBloom = false;
	//トゥーンの有無
	bool isToon = false;
	//アウトラインの有無
	bool isOutline = false;
	//ライティング
	bool isLight = true;
	//インスタンシング描画個数
	int instanceDrawNum = 0;

public:

	static void SetPipeline(const std::vector<GraphicsPipelineManager::DrawSet>& _pipeline) { InstanceObject::pipeline = _pipeline; }
	void SetBloom(bool _isBloom) { this->isBloom = _isBloom; }
	void SetToon(bool _isToon) { this->isToon = _isToon; }
	void SetLight(bool _isLight) { this->isLight = _isLight; }
	void SetOutline(bool _isOutline) { this->isOutline = _isOutline; }
};