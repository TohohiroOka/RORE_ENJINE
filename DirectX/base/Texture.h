#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

#include "DescriptorHeapManager.h"

class Texture
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <param name="cmdList">ddsファイル時はcmdListを持ってくる</param>
	static std::unique_ptr<Texture> Create(const std::string fileName, ID3D12GraphicsCommandList* cmdList = nullptr);

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Texture() {};

	/// <summary>
	/// デストラクタ。
	/// </summary>
	~Texture();

	/// <summary>
	/// テクスチャの読み込み
	/// </summary>
	/// <param name="fileName">画像ファイルの名前</param
	void LoadTexture(const std::string fileName);

	/// <summary>
	/// ddsファイルの読み込み
	/// </summary>
	/// <param name="fileName">画像ファイルの名前</param>
	void LoadTextureFromDDSFile(const std::string fileName, ID3D12GraphicsCommandList* cmdList);

private:

	//デバイス
	static ID3D12Device* device;

public:
	//テクスチャバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuffer = nullptr;
	//テクスチャのデスクリプタ
	std::unique_ptr<DescriptorHeapManager> descriptor = nullptr;
	//ddsのみに使用する
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	Microsoft::WRL::ComPtr<ID3D12Resource> texConstBuffer = nullptr;

};

