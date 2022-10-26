#pragma once
#include "BaseBullet.h"

class BossBulletD :
    public BaseBullet
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_color">色</param>
	BossBulletD(const XMFLOAT3& _pos, const XMFLOAT3& _color);
	~BossBulletD() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_color">色</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletD> Create(const XMFLOAT3& _pos, const XMFLOAT3& _color);

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:

	//タイマー
	int timer;
	//割る値（ランダム）
	int divideNum;
	//回転角
	float angle;
	//速度
	float velocity;
};

