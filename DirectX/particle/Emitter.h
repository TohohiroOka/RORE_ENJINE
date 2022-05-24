#pragma once
#include"ParticleManager.h"

class Camera;

class Emitter
{
private: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;

public://静的メンバ関数

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <param name="name">テクスチャ名</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<Emitter> Create(const std::string name);

public://メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Emitter() {};

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Emitter();

	/// <summary>
	/// パーティクルの発生地点
	/// </summary>
	/// <param name="maxCount">生成する個数の最大</param>
	/// <param name="maxFrame">生存時間</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	/// <param name="accel">加速度</param>
	/// <param name="startScale">初期サイズ</param>
	/// <param name="endScale">最終サイズ</param>
	/// <param name="startColor">初期カラー</param>
	/// <param name="endColor">最終カラー</param>
	void InEmitter(int maxCount, int maxFrame, XMFLOAT3 position, XMFLOAT3 velocity,
		XMFLOAT3 accel, float startScale, float endScale, XMFLOAT4 startColor, XMFLOAT4 endColor);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 削除
	/// </summary>
	void ParticlAllDelete();

	/// <summary>
	/// 生成してあるパーティクルの個数
	/// </summary>
	/// <returns></returns>
	int GetCount() { return count; }

private:
	//パーティクル制御
	int count = 0;
	//パーティクルクラス
	std::unique_ptr<ParticleManager> particleManager = nullptr;
};