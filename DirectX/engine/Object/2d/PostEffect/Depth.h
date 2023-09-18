#pragma once
#include "Object/2d/Sprite.h"

class Depth : public Sprite
{
public: //サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMFLOAT4 color;	//色(RGBA)
		XMMATRIX mat;	//3変換行列
	};

public: //静的メンバ関数
	/// <summary>
	/// シャドウマップ生成
	/// </summary>
	/// <returns>シャドウマップ</returns>
	static std::unique_ptr<Depth> Create();

public: //メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	bool Initialize();

	/// <summary>
	/// シーン描画前処理
	/// </summary>
	void PreDrawScene();

	/// <summary>
	/// シーン描画後処理
	/// </summary>
	void PostDrawScene();

	//getter
	Texture* GetTex() { return texture.get(); }

private: //メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//テクスチャ情報
	std::unique_ptr<Texture> texture;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;

};

