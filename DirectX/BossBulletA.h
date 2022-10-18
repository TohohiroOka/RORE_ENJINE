#pragma once
#include "BaseBullet.h"

class BossBulletA :
    public BaseBullet
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_angleXZ">xz平面の角度</param>
	/// <param name="_angleXY">xy平面の角度</param>
	/// <param name="_color">色</param>
	BossBulletA(const XMFLOAT3& _pos, float _angleXZ, float _angleXY, const XMFLOAT4& _color);
	~BossBulletA() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_angleXZ">xz平面の角度</param>
	/// <param name="_angleXY">xy平面の角度</param>
	/// <param name="_color">色</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletA> Create(const XMFLOAT3& _pos, float _angleXZ, float _angleXY, const XMFLOAT4& _color);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:

	//xz平面の角度
	float angleXZ;
	//xy平面の角度
	float angleXY;
	//中心からの距離
	float distance;

};

