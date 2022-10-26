#pragma once
#include "BaseBullet.h"

class BossBulletE :
	public BaseBullet
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_color">色</param>
	BossBulletE(const XMFLOAT3& _pos, const XMFLOAT3& _color);
	~BossBulletE() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_color">色</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletE> Create(const XMFLOAT3& _pos, const XMFLOAT3& _color);

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:

	//タイマー
	int timer;
	//移動フラグ
	bool isMove;
};