#pragma once
#include "Object3d.h"

class BossBeam
{
protected: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;

public:

	BossBeam() {};
	~BossBeam() {};

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// 生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<BossBeam> Create();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_color">色</param>
	void Update(const XMFLOAT3& _pos, const XMFLOAT3& _color);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// delete処理
	/// </summary>
	static void Finalize();

protected:

	//モデル
	static std::unique_ptr<Model> model;

protected:

	//オブジェクト
	std::unique_ptr<Object3d> object;
	//生存フラグ
	bool isAlive;
	//座標
	XMFLOAT3 pos;
	//ビームの方向
	XMFLOAT3 moveVec;
	//大きさ
	float scale;
	//角度(xy,xz)
	XMFLOAT2 angle;

public:

	void SetIsAlive(bool _isAlive) { isAlive = _isAlive; }
	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
	XMFLOAT3 GetMoveVec() { return moveVec; }

};

