#pragma once
#include "InterfaceScene.h"
#include <array>

class BrowsingCircle : public InterfaceScene
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	///	更新
	/// </summary>
	void Update() override;

	/// <summary>
	///	描画
	/// </summary>
	void Draw() override;

	/// <summary>
	///	解放
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// imguiの表示
	/// </summary>
	void ImguiDraw() override;

	/// <summary>
	/// コンピュートシェーダーからの計算結果取得
	/// </summary>
	void GetConstbufferNum() override;

private:

	//カメラの回転
	float cameraAngle = 100;
	//カメラの高さ
	float cameraY = 50.0f;

	//円モデル
	std::unique_ptr<FbxModel> SpherePBRModel = nullptr;

	//円
	std::array<std::unique_ptr<Fbx>, 25> circle;

};

