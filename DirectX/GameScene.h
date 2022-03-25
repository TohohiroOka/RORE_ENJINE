#pragma once
#include<DirectXMath.h>

#include "Object3d.h"
#include "Sprite.h"
#include "DebugText.h"
#include "Emitter.h"
#include "LightGroup.h"
#include "Fbx.h"
#include "Easing.h"
#include "NormalMap.h"
#include "DrawLine.h"
#include "DrawLine3D.h"

class DirectInput;
class XInputManager;
class Audio;
class Camera;

//ゲームで使用するクラス宣言
class Player;
class Ground;
class TouchableObject;

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

public:// メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene() {};

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	/// <param name="audio">Audioのインスタンス</param>
	/// <param name="camera">Cameraのインスタンス</param>
	void Update(Audio* audio, Camera* camera);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

private:// メンバ変数

	Sprite* sprite = nullptr;
	//DebugText* text;
	Emitter* emit = nullptr;

	Model* uma = nullptr;
	Model* ground = nullptr;
	Model* block = nullptr;

	Player* PLAYER = nullptr;
	Ground* GROUND = nullptr;
	Object3d* BLOCK = nullptr;

	int tex[3];
	NormalMap* water = nullptr;
	bool lightF = false;
	float lightPos = 0;
	float uvPos = 0;

	LightGroup* light = nullptr;

	Fbx* anm = nullptr;
	int FbxUma;

	XMFLOAT3 cameraPosition = { 0,0,50 };

	DrawLine* line = nullptr;
	DrawLine* line_t = nullptr;
	DrawLine3D* line3d[10] = { nullptr };

	int time = 0;
};