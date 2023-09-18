#pragma once
#include "Scene/InterfaceScene.h"
#include "Math/Vector3.h"
#include "Object/3d/HeightMap.h"
#include "Player.h"

class Scene1 : public InterfaceScene
{
private:// エイリアス
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMINT3 = DirectX::XMINT3;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	///	更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 更新
	/// </summary>
	void CameraUpdate(const int _cameraNum, Camera* camera) override;

	/// <summary>
	///	描画
	/// </summary>
	/// <param name="_cameraNum">カメラ番号</param>
	void Draw(const int _cameraNum) override;
	
	/// <summary>
	/// ポストエフェクトをかけない描画
	/// </summary>
	void NonPostEffectDraw(const int _cameraNum) override;

	/// <summary>
	///	解放
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// imguiの表示
	/// </summary>
	void ImguiDraw() override;

	/// <summary>
	/// 
	/// </summary>
	void FrameReset() override;

private:

	Camera* camera;

	//カメラ座標
	XMFLOAT3 cameraPos;
	//カメラ座標
	XMFLOAT3 cameraTarget;

	std::unique_ptr<Sprite> sprite;

	//地形情報
	std::unique_ptr<TerrainModel> m_model;
	//描画設定
	std::unique_ptr<HeightMap> object;

	std::unique_ptr<Player> player;

};