#pragma once
#include "BaseBullet.h"

class BossBulletA : public BaseBullet
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	BossBulletA(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);
	~BossBulletA() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletA> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	void Initialize() override;

	/// <summary>
	/// 色変換
	/// </summary>
	static void ColorChange();

private:

	//球状のダンマクの色操作
	static std::array<bool, 3> isCircleColor;
	//球状のダンマクの色
	static XMFLOAT3 circleColor;

};