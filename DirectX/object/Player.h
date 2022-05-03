#pragma once
#include "Object3d.h"

class Player : public Object3d
{
public:
	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<Player> Create(Model *model = nullptr);
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize() override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// カメラの角度セット
	/// </summary>
	/// <param name="cameraAngle"></param>
	void SetCameraAngle(float cameraAngle) { this->cameraAngle = cameraAngle; }

private:
	bool onGround = true;
	// 落下ベクトル
	DirectX::XMVECTOR fallV;
	//カメラ角度
	float cameraAngle;
};

