#pragma once
#include "BaseBullet.h"

class FixedTurretBullet : public BaseBullet
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	FixedTurretBullet(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);
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
	void Initialize() override;
};