#pragma once
#include <DirectXMath.h>
#include "Camera.h"
#include "Audio.h"

//オブジェクト系
#include "Object3d.h"
#include "Sprite.h"
#include "Emitter.h"
#include "LightGroup.h"
#include "Fbx.h"
#include "Easing.h"
#include "DrawLine.h"
#include "DrawLine3D.h"
#include "CubeMap.h"

//計算シェーダー
#include "ComputeShaderManager.h"

class InterfaceScene
{
protected:// エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	virtual ~InterfaceScene() {};

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 解放
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// imguiの表示
	/// </summary>
	virtual void ImguiDraw() = 0;

	/// <summary>
	/// コンピュートシェーダーからの計算結果取得
	/// </summary>
	virtual void GetConstbufferNum() = 0;

	/// <summary>
	/// コマンドリストのセット
	/// </summary>
	/// <param name="inCmdList">コマンドリスト</param>
	void SetCmdList(ID3D12GraphicsCommandList* inCmdList) { cmdList = inCmdList; }

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラクラスのインスタンス</param>
	void SetCamera(Camera* camera) { this->camera = camera; }

	/// <summary>
	/// ライトのセット
	/// </summary>
	/// <param name="light">ライトクラスのインスタンス</param>
	void SetLight(LightGroup* light) { this->light = light; }

protected:

	//コマンドリスト
	ID3D12GraphicsCommandList* cmdList = nullptr;
	//カメラ
	Camera* camera = nullptr;
	//ライト
	LightGroup* light = nullptr;
};