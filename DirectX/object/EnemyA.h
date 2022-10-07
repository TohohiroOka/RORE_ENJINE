#pragma once
#include "BaseEnemy.h"

class EnemyA : public BaseEnemy
{
public:

	EnemyA() {};
	~EnemyA() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <returns></returns>
	static std::unique_ptr<EnemyA> Create(const XMFLOAT3& _pos);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_pos">座標</param>
	void Initialize(const XMFLOAT3& _pos);

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 敵の移動方向セット
	/// </summary>
	/// <param name="_pos">プレイヤー座標</param>
	void SetMove(const XMFLOAT3& _pos);

	/// <summary>
	/// 押し戻し有の当たり判定
	/// </summary>
	void MapCollider();

private:

	//タイマー
	int timer;
	//オブジェクト
	std::unique_ptr<Object3d> object;
	//生存フラグ
	bool isAlive;
	//座標
	XMFLOAT3 pos;
	//移動ベクトル
	XMFLOAT3 moveVec;
	//大きさ
	float scale;
	//地面についているか
	bool onGround;
	//一回に出す弾の量
	const int bulletNum = 1;

public:

	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
	void SetIsAlive(const bool& _isAlive) { isAlive = _isAlive; }
};

