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
	static std::unique_ptr<Player> Create();
public:

	Player() {};
	~Player() {};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	/// <param name="cameraAngle">カメラ角度</param>
	void Update(float _cameraAngle);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 初期化
	/// </summary>
	void Reset();

	/// <summary>
	/// 押し戻し有の当たり判定
	/// </summary>
	void Collider();

	/// <summary>
	/// 弾のセット
	/// </summary>
	void SetBullet();

private:

	std::unique_ptr<Model> model = nullptr;
	std::unique_ptr<Object3d> object = nullptr;

	XMFLOAT3 position = { 1905,50,25 };
	XMFLOAT3 move = {};

	//カメラ角度
	float cameraAngle;

public:

	/// <summary>
	/// 座標取得
	/// </summary>
	XMFLOAT3 GetPosition() { return object->GetPosition(); }

	/// <summary>
	/// 大きさ取得
	/// </summary>
	XMFLOAT3 GetScale() { return object->GetScale(); }
};