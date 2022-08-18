#pragma once
#include "Object3d.h"

class Player
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;

public:
	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<Player> Create(Model* model = nullptr);
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Draw();

	/// <summary>
	/// カメラの角度セット
	/// </summary>
	/// <param name="cameraAngle"></param>
	void SetCameraAngle(float cameraAngle) { this->cameraAngle = cameraAngle; }

private:

	std::unique_ptr<Object3d> object = nullptr;

	XMFLOAT3 position = { 0,100,0 };

	bool onGround = true;
	// 落下ベクトル
	DirectX::XMVECTOR fallV;
	//カメラ角度
	float cameraAngle;

public:

	XMFLOAT3 GetPos() { return position; }
};