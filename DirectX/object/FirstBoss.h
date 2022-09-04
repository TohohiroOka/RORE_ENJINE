#pragma once
#include "BaseEnemy.h"

class FirstBoss : public BaseEnemy
{
public:

	~FirstBoss() override;

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<FirstBoss> Create();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 座標取得
	/// </summary>
	XMFLOAT3 GetPosition() override;

private://定数

	//ボスの体の数
	static const int bossBodyNum = 8;
	//コアからの距離
	static const XMFLOAT3 bodyDistance[bossBodyNum];

private:

	//ボスのコア
	std::unique_ptr<Object3d> core = nullptr;
	//ボスの体
	std::array<std::unique_ptr<Object3d>, bossBodyNum> body;
};