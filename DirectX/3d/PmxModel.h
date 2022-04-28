#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class PmxModel
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス

	enum HeaderDataIndex
	{
		ENCODING_FORMAT,
		NUMBER_OF_ADD_UV,
		VERTEX_INDEX_SIZE,
		TEXTURE_INDEX_SIZE,
		MATERIAL_INDEX_SIZE,
		BONE_INDEX_SIZE,
		RIGID_BODY_INDEX_SIZE
	};

	enum WeightType
	{
		BDEF1,
		BDEF2,
		BDEF4,
		SDEF,
	};

	struct Weight
	{
		WeightType type;
		int born1;
		int	born2;
		int	born3;
		int	born4;
		float weight1;
		float weight2;
		float weight3;
		float weight4;
		XMFLOAT3 c;
		XMFLOAT3 r0;
		XMFLOAT3 r1;
	};

	//頂点構造体
	struct Vertex
	{
		// 頂点座標
		XMFLOAT3 position;
		// 法線
		XMFLOAT3 normal;
		// UV座標
		XMFLOAT2 uv;
		// 追加UV座標
		std::vector<XMFLOAT4> additionalUV;
		// ボーンウェイト
		Weight weight;
		// エッジ倍率
		float edgeMagnif;
	};

	//面
	struct Surface
	{
		int vertexIndex;
	};

	//マテリアル
	struct Material
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 specular;
		float specularity;
		XMFLOAT3 ambient;

		int colorMapTextureIndex;
		int toonTextureIndex;
		// (スフィアテクスチャは非対応)

		// 材質ごとの頂点数
		int vertexNum;
	};

	//ボーン
	struct Bone
	{
		// ボーン名
		std::string name;
		// English version
		std::string nameEnglish;
		XMFLOAT3 position;
		int parentIndex;
		int transformationLevel;
		unsigned short flag;
		XMFLOAT3 coordOffset;
		int childrenIndex;
		int impartParentIndex;
		float impartRate;
		// 固定軸方向ベクトル
		XMFLOAT3 fixedAxis;
		// ローカルのX軸方向ベクトル
		XMFLOAT3 localAxisX;
		// ローカルのZ軸方向ベクトル
		XMFLOAT3 localAxisZ;
		int externalParentKey;
		int ikTargetIndex;
		int ikLoopCount;
		float ikUnitAngle;
		struct IKLink
		{
			int index;
			bool existAngleLimited;
			XMFLOAT3 limitAngleMin;
			XMFLOAT3 limitAngleMax;
		};
		std::vector<IKLink> ikLinks;
	};

	// 定数バッファ用データ構造体B1
	static const int B1_size = 64;
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient[B1_size]; // アンビエント係数
		float pad1; // パディング
		XMFLOAT3 diffuse[B1_size]; // ディフューズ係数
		float pad2; // パディング
		XMFLOAT3 specular[B1_size]; // スペキュラー係数
		float alpha[B1_size];	// アルファ
	};

private:

	static const std::string baseDirectory;

	static const int NO_DATA_FLAG = -1;
	// デバイス
	static ID3D12Device* device;

public://メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <param name="_filePath">モデルファイルパス</param>
	/// <returns></returns>
	static std::unique_ptr<PmxModel> Create(const std::string& _filePath);

public://メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	PmxModel() {};

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PmxModel() {};

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
	/// <param name="cmdList">命令発行先コマンドリスト</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

private://メンバ変数

	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// テクスチャバッファ
	std::vector<ComPtr<ID3D12Resource>> texBuffer;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuffB1;
	//頂点配列
	std::vector<Vertex> vertices;
	//面
	std::vector<Surface> surfaces;
	//テクスチャパス
	std::vector<std::string> texturePaths;
	//マテリアル
	std::vector<Material> materials;
	//ボーン
	std::vector<Bone> bones;

	private://メンバ関数

	/// <summary>
	/// 頂点の読み込み
	/// </summary>
	/// <param name="pmxFile">ファイルデータ</param>
	/// <param name="hederData">ファイル識別データ</param>
	/// <returns>頂点配列</returns>
		static std::vector<Vertex> CollectVertex(std::ifstream& pmxFile, std::array<byte, 8> hederData);

	/// <summary>
	/// 面の読み込み
	/// </summary>
	/// <param name="pmxFile">ファイルデータ</param>
	/// <param name="hederData">ファイル識別データ</param>
	/// <returns>面配列</returns>
	static std::vector<Surface> CollectSurface(std::ifstream& pmxFile, std::array<byte, 8> hederData);

	/// <summary>
	/// テクスチャの読み込み
	/// </summary>
	/// <param name="pmxFile">ファイルデータ</param>
	/// <param name="folderPath">フォルダパス</param>
	/// <returns>テクスチャ配列</returns>
	static std::vector<std::string> CollectTexturePaths(std::ifstream& pmxFile, std::string folderPath);

	/// <summary>
	/// マテリアルの読み込み
	/// </summary>
	/// <param name="pmxFile">ファイルデータ</param>
	/// <param name="hederData">ファイル識別データ</param>
	/// <param name="arrayLength">配列数</param>
	/// <returns>マテリアル配列</returns>
	static std::vector<Material> CollectMaterial(std::ifstream& pmxFile,
		std::array<byte, 8> hederData, unsigned arrayLength);

	/// <summary>
	/// ボーンの読み込み
	/// </summary>
	/// <param name="pmxFile">ファイルデータ</param>
	/// <param name="hederData">ファイル識別データ</param>
	/// <param name="arrayLength">配列数</param>
	/// <returns>ボーン配列</returns>
	static std::vector<Bone> CollectBone(std::ifstream& pmxFile,
		std::array<byte, 8> hederData, unsigned arrayLength);

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTexture();

public://メンバ関数

	/// <summary>
	/// PMXモデルデータのロード関数
	/// </summary>
	/// <param name="_filePath">モデルファイルパス</param>
	/// <param name="vertices">頂点配列</param>
	/// <param name="surfaces">面配列</param>
	/// <param name="texturePaths">テクスチャ配列</param>
	/// <param name="materials">マテリアル配列</param>
	/// <param name="bones">ボーン配列</param>
	static void LoadPMX(const std::string& _filePath, std::vector<Vertex>& vertices,
		std::vector<Surface>& surfaces, std::vector<std::string>& texturePaths,
		std::vector<Material>& materials, std::vector<Bone>& bones);

};