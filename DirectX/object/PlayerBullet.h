#pragma once
#include "BaseBullet.h"

class PlayerBullet : public BaseBullet
{
private:

	struct INFORMATION
	{
		//インスタンス
		std::unique_ptr<Object3d> instance = nullptr;
		//生存フラグ
		bool isAlive = false;
		//移動ベクトル
		XMFLOAT3 vecMove = {};
	};

public:

	~PlayerBullet() override;

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<PlayerBullet> Create();

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
	/// 弾発射時の設定
	/// </summary>
	/// <param name="_pos">初期値</param>
	/// <param name="_vecMove">移動ベクトル</param>
	void SetMove(const XMFLOAT3 _pos, const XMFLOAT3 _vecMove);

	/// <summary>
	/// 初期化
	/// </summary>
	void Reset();

private:

	const int bulletNum = 50;
	//弾
	std::vector<INFORMATION> object;
};

