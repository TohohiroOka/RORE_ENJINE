#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dx12.h>
#include <cstdlib>

#include "Singleton.h"

class DirectXCommon : public Singleton< DirectXCommon >
{
	friend Singleton< DirectXCommon >;

private:
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private://コンストラクタ&デストラクタ

	DirectXCommon() {};
	~DirectXCommon();

public://メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepth();

	/// <summary>
	/// 描画前設定
	/// </summary>
	void BeforeDraw();

	/// <summary>
	/// 描画後設定
	/// </summary>
	void AfterDraw();

	/// <summary>
	/// デバイスのgetter
	/// </summary>
	/// <returns>デバイス</returns>
	static ID3D12Device* GetDevice() { return device.Get(); }

	/// <summary>
	/// コマンドリストのgetter
	/// </summary>
	/// <returns>コマンドリスト</returns>
	static ID3D12GraphicsCommandList* GetCmdList() { return cmdList.Get(); }

private://静的メンバ変数

	//デバイス
	static ComPtr<ID3D12Device> device;
	//コマンドリスト
	static ComPtr<ID3D12GraphicsCommandList> cmdList;

private://メンバ変数

	//ファクトリー
	ComPtr<IDXGIFactory6> dxgiFactory;
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> cmdQueue;
	//スワップチェーン
	ComPtr<IDXGISwapChain4> swapchain;
	//バックバッファ
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	//レンダーターゲットビュー用ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	//深度バッファ
	ComPtr<ID3D12Resource> depthBuffer;
	//深度ステンシルビュー用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	//深度ビュー用のデスクリプタヒープ
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	//フェンス
	ComPtr<ID3D12Fence> fence;
	//コマンドリスト完了までのカウント
	UINT64 fenceVal = 0;
};

