#pragma once
#include "InstanceObject.h"

class BaseBullet
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
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	static void Draw();

	/// <summary>
	/// delete処理
	/// </summary>
	static void Finalize();

protected:

	//モデル
	static std::unique_ptr<Model> model;
	//オブジェクトの最大値
	static const int object_max_num = 20;
	//オブジェクト
	static std::array<std::unique_ptr<InstanceObject>, object_max_num> object;
	//現在の使用個数
	static int usingNum;

protected:

	//生存フラグ
	bool isAlive;
	//座標
	XMFLOAT3 pos;
	//移動ベクトル
	XMFLOAT3 moveVec;
	//移動速度
	XMFLOAT3 move;
	//大きさ
	float scale;
	//角度
	XMFLOAT3 rotate;
	//色
	XMFLOAT3 color = { 1,1,1 };

public:

	void SetIsAlive(bool _isAlive) { isAlive = _isAlive; }
	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
	XMFLOAT3 GetMoveVec() { return moveVec; }
	XMFLOAT3 GetMove() { return move; }
};

