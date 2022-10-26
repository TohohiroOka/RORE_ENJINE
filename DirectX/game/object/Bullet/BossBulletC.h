#pragma once
#include "BaseBullet.h"

class BossBulletC :
    public BaseBullet
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_targetPos">ターゲット座標</param>
	/// <param name="_color">色</param>
	BossBulletC(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, const XMFLOAT3& _color);
	~BossBulletC() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_targetPos">ターゲット座標</param>
	/// <param name="_color">色</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletC> Create(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, const XMFLOAT3& _color);
};

