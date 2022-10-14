#pragma once
#include "Object3d.h"

class BaseEnemy
{
protected: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;

public:

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// delete処理
	/// </summary>
	static void Finalize();

protected:

	//モデル
	static std::unique_ptr<Model> model;

protected:

	//オブジェクト
	std::unique_ptr<Object3d> object;
	//生存フラグ
	bool isAlive;
	//HP
	int hp;
	//座標
	XMFLOAT3 pos;
	//移動ベクトル
	XMFLOAT3 moveVec;
	//大きさ
	float scale;

public:

	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
	void SetIsAlive(bool _isAlive) { isAlive = _isAlive; }
	void Damage() { hp--; }
};