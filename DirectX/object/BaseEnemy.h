#pragma once
#include "Object3d.h"
#include <array>

class BaseEnemy
{
protected: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BaseEnemy() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseEnemy();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 座標取得
	/// </summary>
	/// <returns>座標</returns>
	virtual XMFLOAT3 GetPosition() = 0;

	///// <summary>
	///// 死亡
	///// </summary>
	//void Dead() = 0;

protected://定数

	static const int modelNum = 2;

protected:

	std::array<std::unique_ptr<Model>, modelNum> model;
};