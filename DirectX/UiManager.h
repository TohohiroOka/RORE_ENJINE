#pragma once
#include "BossHpUi.h"

class UiManager
{
public:

	UiManager() {};
	~UiManager() {};

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="_bossMaxHp">ボス最大HP</param>
	/// <returns></returns>
	static std::unique_ptr<UiManager> Create(const int _bossMaxHp);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_bossMaxHp">ボス最大HP</param>
	void Initialize(const int _bossMaxHp);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="_bossHp">ボス最大HP</param>
	void Update(const int _bossHp);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:

	std::unique_ptr<BossHpUi> bossHp;
};