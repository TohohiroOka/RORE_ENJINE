#pragma once
#include "InterfaceScene.h"

class SceneManager
{
public://静的メンバ関数

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<SceneManager> Create();

public://メンバ関数

	SceneManager() {};
	~SceneManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	bool Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// imguiの表示
	/// </summary>
	void ImguiDraw();

	/// <summary>
	/// コンピュートシェーダーからの計算結果取得
	/// </summary>
	void GetConstbufferNum();

	/// <summary>
	/// 次シーンへの移行
	/// </summary>
	/// <param name="inScene">次シーンのインスタンス</param>
	static void SetNextScene(InterfaceScene* inScene) { nextScene = inScene; }

private://メンバ変数

	//現在シーン
	static std::unique_ptr<InterfaceScene> scene;
	//次シーン
	static InterfaceScene* nextScene;
	//Cameraのインスタンス
	std::unique_ptr<Camera> camera = nullptr;
	//オーディオ
	std::unique_ptr<Audio> audio = nullptr;
	//ライト
	std::unique_ptr<LightGroup> light = nullptr;
};