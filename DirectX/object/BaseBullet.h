#pragma once
#include "Object3d.h"

class BaseBullet
{
protected: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BaseBullet() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseBullet() {};

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

protected:

	std::unique_ptr<Model> model = nullptr;
};

