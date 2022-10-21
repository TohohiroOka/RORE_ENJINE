#pragma once
#include "BaseBullet.h"

class BossBulletC :
    public BaseBullet
{
private:

	enum class EASING {
		LERP,
		IN_QUART,
		OUT_QUART,
		IN_EXPO,
		OUT_EXPO,
		IN_CIRC,
		OUT_CIRC,
		SIZE,
	};

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_targetPos">ターゲット座標</param>
	BossBulletC(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos);
	~BossBulletC() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_targetPos">ターゲット座標</param>
	/// <param name="_color">色</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletC> Create(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, const XMFLOAT3& _color);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_moveVec">移動量</param>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 使うイージングの振り分け
	/// </summary>
	/// <param name="kind">イージング番号</param>
	/// <param name="_startPos">スタート座標</param>
	/// <param name="_targetPos">ターゲット座標</param>
	/// <param name="rate">進み具合</param>
	/// <returns>座標</returns>
	float EasingDivide(EASING kind, float _startPos, float _targetPos, float rate);

private:

	int timer;
	XMFLOAT3 startPos;
	XMFLOAT3 targetPos;
	EASING easingKind[3];
	//保持を行うかのフラグ
	bool isStorage;
	//イージング以降の動きを決めるための保持座標
	XMFLOAT3 storagePos;

	const float countPerMeter = 1.0f;
	const float speed = 1.0f;
	const float curvatureRadius = 1.0f;
	const float damping = 0.1f;
	const float impact = 1.0f;
	float maxCentripetalAccel;
	float propulsion; // 推進力
};

