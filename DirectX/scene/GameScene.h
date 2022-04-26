#pragma once
#include <DirectXMath.h>
#include <memory>

#include "Audio.h"
#include "Object3d.h"
#include "Sprite.h"
#include "Emitter.h"
#include "LightGroup.h"
#include "Fbx.h"
#include "Easing.h"
#include "NormalMap.h"
#include "DrawLine.h"
#include "DrawLine3D.h"
#include "ComputeShaderManager.h"

#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"

class DirectInput;
class XInputManager;
class Audio;
class Camera;

class GameScene
{
private:// エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://静的メンバ関数

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<GameScene> Create();

public:// メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene() {};

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene() {};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	/// <param name="camera">Cameraのインスタンス</param>
	void Update(Camera* camera);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// コンピュートシェーダーからの計算結果取得
	/// </summary>
	void GetConstbufferNum();

private:// メンバ変数

	//音
	std::unique_ptr<Audio> audio = nullptr;

	//スプライト
	std::unique_ptr<Sprite> sprite = nullptr;

	//パーティクル
	std::unique_ptr<Emitter> emit = nullptr;

	//モデル
	std::unique_ptr<Model> uma = nullptr;
	std::unique_ptr<Model> ground = nullptr;
	std::unique_ptr<Model> block = nullptr;

	//自作クラス
	std::unique_ptr<Player> PLAYER = nullptr;
	std::unique_ptr<Ground> GROUND = nullptr;

	//Obj
	std::unique_ptr<Object3d> BLOCK = nullptr;

	//NormalMap
	std::unique_ptr<NormalMap> water = nullptr;
	int tex[3];
	bool lightF = false;
	float lightPos = 0;
	float uvPos = 0;

	//ライト
	std::unique_ptr<LightGroup> light = nullptr;

	//FbxModel
	std::unique_ptr<FbxModel> danceModel = nullptr;

	//Fbx
	std::unique_ptr<Fbx> anm = nullptr;

	//カメラ座標
	XMFLOAT3 cameraPosition = { 0,0,50 };

	//線
	std::unique_ptr<DrawLine> line = nullptr;
	std::unique_ptr<DrawLine> line_t = nullptr;
	std::unique_ptr<DrawLine3D> line3d = nullptr;

	//コンピュートシェーダー
	std::unique_ptr<ComputeShaderManager> compute = nullptr;
	static const int max = 10;

	XMFLOAT3 startPosition[max];//初期座標
	XMFLOAT3 endPosition[max];//終了座標
	XMFLOAT3 nowPosition[max];//現在座標
	float time[max];//時間
};