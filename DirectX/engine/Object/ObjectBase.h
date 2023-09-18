#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

#include "Pipeline/GraphicsPipelineManager.h"
#include "Texture/Texture.h"

class ObjectBase
{
protected:// エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

protected:

	enum class DrawMode {
		alpha,
		add,
	};

public:

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="_device">デバイス</param>
	static void StaticInitialize(ID3D12Device* _device);

	/// <summary>
	/// コマンドリストのセット
	/// </summary>
	/// <param name="_cmdList">コマンドリスト</param>
	static void SetCmdList(ID3D12GraphicsCommandList* _cmdList) { ObjectBase::cmdList = _cmdList; }

	/// <summary>
	/// コマンドリストのセット
	/// </summary>
	static void ReleaseCmdList() { ObjectBase::cmdList = nullptr; }

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw(const GraphicsPipelineManager::DrawSet& _drawSet);

protected:

	// デバイス
	static ID3D12Device* device;
	//コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;

};