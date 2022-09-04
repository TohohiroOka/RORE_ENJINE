#pragma once
#include "Object3d.h"
#include "PlayerBullet.h"

class Player
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;

public:
	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<Player> Create();
public:

	/// <summary>
	/// 初期化
	/// </summary>
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
	/// 弾の更新
	/// </summary>
	/// <param name="target">撃つ方向</param>
	void BulletUpdate(const XMFLOAT3 target);

private:

	std::unique_ptr<Model> model = nullptr;
	std::unique_ptr<Object3d> object = nullptr;

	bool onGround = true;
	// 落下ベクトル
	DirectX::XMVECTOR fallV;

	//ターゲットしているか
	static bool isTargeting;
	//ターゲットの座標
	static XMFLOAT3 targetPos;
	//カメラ角度
	float cameraAngle;

	//移動角度
	float moveAngle = 0;
	//移動距離
	float moveDistance = 0;

	//弾
	std::unique_ptr<PlayerBullet> bullet = nullptr;

public:

	/// <summary>
	/// 座標取得
	/// </summary>
	XMFLOAT3 GetPos() { return object->GetPosition(); }

	/// <summary>
	/// ターゲット座標セット
	/// </summary>
	/// <param name="target">ターゲットの座標</param>
	static void SetTarget(XMFLOAT3& _target) {
		isTargeting = (isTargeting + 1) % 2;
		targetPos = _target;
	};

	/// <summary>
	/// カメラの角度セット
	/// </summary>
	/// <param name="cameraAngle">カメラ角度</param>
	void SetCameraAngle(float cameraAngle) { this->cameraAngle = cameraAngle; }

	/// <summary>
	/// ターゲットしているか
	/// </summary>
	/// <returns>ターゲットフラグ</returns>
	bool GetIsTargeting() { return isTargeting; }

	/// <summary>
	/// ターゲット座標取得
	/// </summary>
	/// <returns>ターゲット座標</returns>
	XMFLOAT3 GetTarget() { return targetPos; }

};