#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"

class TestField : public InterfaceScene
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	///	更新
	/// </summary>
	void Update() override;

	/// <summary>
	///	描画
	/// </summary>
	void Draw() override;

	/// <summary>
	///	解放
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// imguiの表示
	/// </summary>
	void ImguiDraw() override;

	/// <summary>
	/// コンピュートシェーダーからの計算結果取得
	/// </summary>
	void GetConstbufferNum() override;

private:

	//カメラの回転
	float cameraAngle = 100;
	//カメラの高さ
	float cameraY = 50.0f;

	//スプライト
	std::unique_ptr<Sprite> sprite = nullptr;

	//パーティクル
	std::unique_ptr<Emitter> emit = nullptr;

	//モデル
	std::unique_ptr<Model> uma = nullptr;
	std::unique_ptr<Model> ground = nullptr;
	std::unique_ptr<Model> block = nullptr;

	//自作クラス
	std::unique_ptr<Player> PLAYER = nullptr;
	std::unique_ptr<Ground> GROUND = nullptr;

	//Obj
	std::unique_ptr<Object3d> BLOCK = nullptr;

	//FbxModel
	std::unique_ptr<FbxModel> danceModel = nullptr;
	std::unique_ptr<FbxModel> SpherePBRModel = nullptr;
	std::unique_ptr<FbxModel> SpiralPBRModel = nullptr;

	//Fbx
	std::unique_ptr<Fbx> anm = nullptr;

	//カメラ座標
	XMFLOAT3 cameraPosition = { 0,0,50 };

	//線
	std::unique_ptr<DrawLine> line = nullptr;
	std::unique_ptr<DrawLine> line_t = nullptr;
	std::unique_ptr<DrawLine3D> line3d = nullptr;

	//コンピュートシェーダー
	std::unique_ptr<ComputeShaderManager> compute = nullptr;
	static const int max = 10;

	XMFLOAT3 startPosition[max];//初期座標
	XMFLOAT3 endPosition[max];//終了座標
	XMFLOAT3 nowPosition[max];//現在座標
	float time[max];//時間

};

