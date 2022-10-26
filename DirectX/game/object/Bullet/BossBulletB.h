#pragma once
#include "BaseBullet.h"
#include "BossBulletA.h"

class BossBulletB :
    public BaseBullet
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	/// <param name="_color">色</param>
	BossBulletB(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color);
	~BossBulletB() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	/// <param name="_color">色</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletB> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color);

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:

	//タイマー
	int timer;
};

