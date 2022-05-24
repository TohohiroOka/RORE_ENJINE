#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class DescriptorHeapManager
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 描画前準備
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	DescriptorHeapManager() {};

	/// <summary>
	/// デストラクタ。
	/// </summary>
	~DescriptorHeapManager() {};

	/// <summary>
	/// シェーダーリソースビューの作成
	/// </summary>
	/// <param name="texBuffer">テクスチャバッファ</param>
	/// <param name="srvDesc">シェーダーリソースビュー設定</param>
	void CreateSRV(Microsoft::WRL::ComPtr<ID3D12Resource> texBuffer, D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc);

private:

	//デバイス
	static ID3D12Device* device;
	//デスクリプタヒープ
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;

public:

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpu;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpu;
};

