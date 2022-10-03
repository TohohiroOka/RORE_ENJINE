#pragma once
#include "Object3d.h"

class FixedTurret
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;

public:

	FixedTurret() {};
	~FixedTurret() {};

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	/// <returns></returns>
	static std::unique_ptr<FixedTurret> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	void Initialize(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 初期化
	/// </summary>
	void Reset();
	
private:

	static std::unique_ptr<Model> model;
	std::unique_ptr<Object3d> object;
	//座標
	XMFLOAT3 pos;
	//発射方向
	bool isAddSub;
	XMFLOAT3 moveVec;

	//発射時間
	int timer;
};

