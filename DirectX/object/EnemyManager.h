#pragma once
#include "BaseEnemy.h"

class EnemyManager
{
private: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	
public:
	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<EnemyManager> Create();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Draw();

	/// <summary>
	/// ターゲットのセット
	/// </summary>
	void SetTarget();

private:

	std::unique_ptr<BaseEnemy> enemy = nullptr;


};

