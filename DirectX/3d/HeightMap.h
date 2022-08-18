#pragma once
#include "InterfaceObject3d.h"

class BaseCollider;
class Camera;
class LightGroup;

class HeightMap : public InterfaceObject3d
{
private: // エイリアス

	struct HeightMapInfo {		// Heightmap structure
		int terrainWidth;		// Width of heightmap
		int terrainHeight;		// Height (Length) of heightmap
		std::vector<XMFLOAT3> heightMap;	// Array to store terrain's vertex positions
	};

	enum TEXTURE {
		HeightMapTex,
		GraphicTex1,
		GraphicTex2,
		Size,
	};

public://メンバ関数

	/// <summary>
	/// directXCommon生成
	/// </summary>
	/// <param name="heightmapFilename">heightmap名</param>
	/// <param name="filename">ファイル名</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<HeightMap> Create(const std::string heightmapFilename,
		const std::string filename1 = "null", const std::string filename2 = "null");

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw();

private://メンバ関数

	/// <summary>
	/// ハイトマップ
	/// </summary>
	/// <param name="filename">ファイル名</param>
	/// <returns>成功か</returns>
	bool HeightMapLoad(const std::string filename);

	/// <summary
	/// テクスチャ読み込み
	/// </summary>
	/// <param name="filename1">ファイル名</param>
	/// <param name="filename2">ファイル名</param>
	void LoadTexture(const std::string filename1, const std::string filename2);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

public:

	HeightMap() {};
	~HeightMap();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	/// <param name="pipeline"></param>
	static void SetPipeline(GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline) { HeightMap::pipeline = pipeline; }

private:

	//パイプライン
	static GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline;
	//基礎ファイル
	static const std::string baseDirectory;

private:

	//テクスチャ情報
	std::array<std::unique_ptr<Texture>, TEXTURE::Size> texture;
	// モデル
	Model* model = nullptr;
	//インデックスの大きさ
	int indexNum = 0;
	//頂点数
	int vertNum = 0;
	//ハイトマップの情報
	HeightMapInfo hmInfo;

public:

	Model* GetModel() { return model; }

};