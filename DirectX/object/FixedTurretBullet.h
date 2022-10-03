#pragma once
#include "BaseBullet.h"

class FixedTurretBullet : public BaseBullet
{
public:

	FixedTurretBullet() {};
	~FixedTurretBullet() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	/// <returns></returns>
	static std::unique_ptr<FixedTurretBullet> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	void Initialize(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

private:

	std::unique_ptr<Object3d> object;
	//生存フラグ
	bool isAlive;
	//座標
	XMFLOAT3 pos;
	//移動ベクトル
	XMFLOAT3 moveVec;
	//大きさ
	float scale;

public:

	void SetIsAlive(bool _isAlive) { isAlive = _isAlive; }
	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
};