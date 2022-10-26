#pragma once
#include "BaseBullet.h"

class BossBulletF : public BaseBullet
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	/// <param name="_color">色</param>
	BossBulletF(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color);
	~BossBulletF() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	/// <param name="_color">色</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletF> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color);

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:

	int timer;
	int maxTimer;
};