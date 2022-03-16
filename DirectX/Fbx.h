#pragma once
#include<fbxsdk.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include<DirectXTex.h>
#include<map>

class Camera;

class Fbx
{
protected:// エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://固定値
	//ボーンの最大値
	static const int MAX_BONES = 32;

private://構造体宣言
	static const int MAX_BONE_INDICES = 4;
	//頂点データ3D
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
		UINT boneIndex[MAX_BONE_INDICES];
		float boneWhight[MAX_BONE_INDICES];
	};

	//ノード
	struct Node
	{
		//名前
		std::string name;
		//ローカルスケール
		DirectX::XMVECTOR scaling = { 1.0f,1.0f,1.0f,0.0f };
		//ローカル回転
		DirectX::XMVECTOR rotation = { 0.0f,0.0f,0.0f,0.0f };
		//ローカル移動
		DirectX::XMVECTOR translation = { 0.0f,0.0f,0.0f,1.0f };
		//ローカル変形行列
		DirectX::XMMATRIX transform;
		//グローバル変形行列
		DirectX::XMMATRIX globalTransform;
		//親ノード
		Node* parent = nullptr;
	};

	//ボーン
	struct Bone
	{
		//名前
		std::string name;

		//初期姿勢行列
		DirectX::XMMATRIX invInitialPose;

		//クラスター
		FbxCluster* fbxCluster;

		//コンストラクタ
		Bone(const std::string& name)
		{
			this->name = name;
		}
	};

	//マテリアル用
	struct Material
	{
		std::string name;//マテリアル名
		XMFLOAT3 ambient;//アンビエント影響度
		XMFLOAT3 diffuse;//ディフューズ影響度
		XMFLOAT3 emissive;// エミッシブ
		XMFLOAT3 bump;// バンプ
		XMFLOAT3 specular;//スペキュラ影響度
		float alpha;//アルファ
		std::string textureFilename;//テクスチャファイル名
		//コンストラクタ
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.3f,0.3f,0.3f };
			specular = { 0.3f,0.3f,0.3f };
			alpha = 1.0f;
		}
	};

	//定数バッファ用データB0
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;//ビュープロジェクション行列
		XMMATRIX world;//ワールド行列
		XMFLOAT3 cameraPos;//カメラ座標（ワールド座標）
		XMFLOAT4 color;//色
	};

	//スキン用定数バッファデータ
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

	struct FbxUpdate
	{
		bool isAnimation = false;//アニメーション可能か
		FbxScene* fbxScene = nullptr;
		FbxTime startTime;//フレームのスタート
		FbxTime stopTime;;//フレームの最後
		FbxTime nowTime;//現在の進行フレーム
	};

	//Fbxデータ
	struct Data
	{
		Material material;
		std::vector<Vertex> vertices;
		std::vector<unsigned short>indices;
		std::vector<Node> nodes;
		std::vector<Bone> bones;
		FbxUpdate fbxUpdate;
	};

private://静的メンバ関数関数

	/// <summary>
	/// パイプライン設定
	/// </summary>
	static void Pipeline();

	/// <summary>
	/// ノード読み込み
	/// </summary>
	/// <param name="dataNumber">格納する配列番号</param>
	/// <param name="fbxNode">ノード</param>
	/// <param name="parent">親ノード</param>
	static void LoadNode(int dataNumber, FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// メッシュを探す
	/// </summary>
	/// <param name="dataNumber">格納する配列番号</param>
	/// <param name="fbxNode">ノード</param>
	static void CollectMesh(int dataNumber, FbxNode* fbxNode);

	/// <summary>
	/// 頂点読み込み
	/// </summary>
	/// <param name="dataNumber">格納する配列番号</param>
	/// <param name="fbxMesh">メッシュ</param>
	static void CollectVertices(int dataNumber, FbxMesh* fbxMesh);

	/// <summary>
	/// 面ごとの読み込み
	/// </summary>
	/// <param name="dataNumber">格納する配列番号</param>
	/// <param name="fbxMesh">メッシュ</param>
	static void CollectMeshFaces(int dataNumber, FbxMesh* fbxMesh);

	/// <summary>
	/// スキニング情報の読み取り
	/// </summary>
	/// <param name="dataNumber">格納する配列番号</param>
	/// <param name="fbxMesh">メッシュ</param>
	static void CollectSkin(int dataNumber, FbxMesh* fbxMesh);

	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	/// <param name="dataNumber">格納する配列番号</param>
	/// <param name="fbxNode">ノード</param>
	static void LoadMaterial(int dataNumber, FbxNode* fbxNode);

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <param name="dataNumber">格納する配列番号</param>
	/// <param name="fileName">ファイル名</param>
	static void LoadTexture(int dataNumber, const std::string fileName);

	/// <summary>
	/// アニメーション読み込み
	/// </summary>
	/// <param name="dataNumber">格納する配列番号</param>
	static void LoadAnimation(int dataNumber);

	/// <summary>
	/// //行列の変換
	/// </summary>
	/// <param name="dst">格納するXMMATRIX型変数</param>
	/// <param name="src">変換するFbxMatrix型変数</param>
	static void ConvertMatrixFormFbx(DirectX::XMMATRIX* dst, const FbxMatrix& src);

	/// <summary>
	/// //ファイル名抽出
	/// </summary>
	/// <param name="path">ファイルパス</param>
	/// <returns></returns>
	static std::string ExtractFileName(const std::string& path);

public://静的メンバ関数
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <returns></returns>
	Fbx() {};

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Fbx();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// Fbxファイルの読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <returns>保存番号</returns>
	static int LoadFbx(const std::string fileName);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

public:

	/// <summary>
	/// 格納されているFBXデータを元にデータを作成する
	/// </summary>
	/// <param name="modelNumber">保存番号</param>
	void CreateModel(UINT modelNumber);

	/// <summary>
	/// 生成
	/// </summary>
	void Create();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="camera">カメラのインスタンス</param>
	void Update(Camera* camera);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="modelNumber"></param>
	void Draw(int modelNumber);

	/// <summary>
	/// 情報のセット
	/// </summary>
	/// <param name="position"></param>
	/// <param name="rotation"></param>
	/// <param name="scale"></param>
	/// <param name="isAnimation"></param
	void SetInformation(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, bool isAnimation) {
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
		this->isAnimation = isAnimation;
	}

	//ワールド行列の取得
	const XMMATRIX& GetMatWorld() { return matWorld; }


private://静的メンバ変数
	static FbxManager* fbxManager;//Fbxの基盤
	static FbxTime frameTime;//1フレームの時間
	static ID3D12Device* device;//デバイス
	static ID3D12GraphicsCommandList* cmdList;//コマンドリスト
	static std::vector<Data> data;//Fbxデータの格納場所
	static ComPtr<ID3D12PipelineState>pipelinestate;//パイプラインステートオブジェクト
	static ComPtr<ID3D12RootSignature>rootsignature;//ルートシグネチャ
	static ComPtr<ID3D12DescriptorHeap>descHeap;//テクスチャ用デスクリプタヒープの生成
	static const int textureNum = 512;//テクスチャ最大登録数
	static ComPtr<ID3D12Resource>texBuffer[textureNum];//テクスチャリソース(テクスチャバッファ)の配列
	static int vecSize;//現在の配列数
	static const std::string subTexture;//textureが無い時のtexture
	static const std::string directoryPath;//ファイルパス
	static std::string fileName;//ファイルネームの保持

private://動的メンバ変数
	UINT modelNumber = -1;//テクスチャ番号
	ComPtr<ID3D12Resource> constBuffTransform;//定数バッファ
	ComPtr<ID3D12Resource> constBuffSkin;//定数バッファ
	ComPtr<ID3D12Resource> vertBuff;//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView;//頂点バッファビュー
	ComPtr<ID3D12Resource> indexBuff;//インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW ibView;//インデックスバッファビュー
	XMMATRIX matWorld;// ローカルワールド変換行列
	XMFLOAT3 position = {};//座標
	XMFLOAT3 rotation = {};//回転角
	XMFLOAT3 scale = {};//大きさ
	bool isAnimation = false;//アニメーションするか
};