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
	static std::unique_ptr<Player> Create(const XMFLOAT3& _pos);

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">座標</param>
	Player(const XMFLOAT3& _pos);
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

	XMFLOAT3 position;
	XMFLOAT3 move;
	XMFLOAT3 speed;

	//描画するか否か
	bool isDraw;

	//移動時のオブジェクトの回転角
	XMFLOAT3 moveObjAngle;

	//カメラ角度
	float cameraAngle;

	//HP
	int hp;

	//ダメージ演出
	bool isDamageStaging;
	int damageTimer;

public:

	/// <summary>
	/// 座標取得
	/// </summary>
	XMFLOAT3 GetPosition() { return object->GetPosition(); }

	/// <summary>
	/// 大きさ取得
	/// </summary>
	XMFLOAT3 GetScale() { return object->GetScale(); }

	void Damage() {
		damageTimer = 0;
		isDamageStaging = true;
		hp--;
	}
	bool GetIsAlive(){
		return hp > 0;
	}

	/// <summary>
	/// オブジェクトの正規化傾き
	/// </summary>
	/// <returns></returns>
	XMFLOAT3 GetObjAngle() { return { moveObjAngle.x / 45.0f, moveObjAngle.y / 45.0f, moveObjAngle.z / 45.0f }; }

};