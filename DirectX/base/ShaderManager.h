#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

class BLOB_OBJECT
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BLOB_OBJECT() {};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	~BLOB_OBJECT() {
		vs.Reset();
		ps.Reset();
		gs.Reset();
		cs.Reset();
	};

public:
	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> vs;
	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> ps;
	// ジオメトリシェーダオブジェクト
	ComPtr<ID3DBlob> gs;
	// コンピュートシェーダオブジェクト
	ComPtr<ID3DBlob> cs;
};

class ShaderManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	ShaderManager() {};
	~ShaderManager() {};

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<ShaderManager> Create();

	/// <summary>
	/// シェーダーの読み込み
	/// </summary>
	void LoadShader();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

public:

	//Objのシェーダーオブジェクト
	std::unique_ptr<BLOB_OBJECT> obj = nullptr;
	//Spriteのシェーダーオブジェクト
	std::unique_ptr<BLOB_OBJECT> sprite = nullptr;
	//Fbxのシェーダーオブジェクト
	std::unique_ptr<BLOB_OBJECT> fbx = nullptr;
	//DrawLine2dのシェーダーオブジェクト
	std::unique_ptr<BLOB_OBJECT> drawLine2d = nullptr;
	//DrawLine3dのシェーダーオブジェクト
	std::unique_ptr<BLOB_OBJECT> drawLine3d = nullptr;
	//Fbxのシェーダーオブジェクト
	std::unique_ptr<BLOB_OBJECT> postEffect = nullptr;
	//NormalMapのシェーダーオブジェクト
	std::unique_ptr<BLOB_OBJECT> normalMap = nullptr;
	//Particleのシェーダーオブジェクト
	std::unique_ptr<BLOB_OBJECT> particle = nullptr;
	//Computeのシェーダーオブジェクト
	std::unique_ptr<BLOB_OBJECT> Compute = nullptr;
};

