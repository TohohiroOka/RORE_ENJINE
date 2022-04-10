#include "MainEngine.h"
#include "Object3d.h"
#include "Sprite.h"
#include "DrawLine.h"
#include "DrawLine3D.h"
#include "Emitter.h"
#include "Fbx.h"
#include "NormalMap.h"
#include "SafeDelete.h"
#include "ComputeShaderManager.h"

MainEngine::~MainEngine()
{
	winApp->Release();
	safe_delete(winApp);
}

void MainEngine::AllDelete()
{
	safe_delete(scene);
	safe_delete(camera);
	safe_delete(postEffect);
	safe_delete(dXCommon);
	safe_delete(audio);
	safe_delete(dXCommon);
}

void MainEngine::Initialize(const wchar_t* gameName, int window_width, int window_height)
{
	//ウィンドウ初期化
	winApp = new WindowApp();
	winApp->Initialize(window_width, window_height, gameName);

	//1フレームの時間設定
	QueryPerformanceFrequency(&timeFreq);

	// 1度取得しておく(初回計算用)
	QueryPerformanceCounter(&timeStart);

	//directX初期化
	dXCommon = new DirectXCommon();
	dXCommon->Initialize();

	//key
	input = DirectInput::GetInstance();
	input->Initialize();

	//パッド
	xinput = XInputManager::GetInstance();
	xinput->Initialize();

	//カメラの初期化
	camera = new Camera();

	//Object系の初期化
	Object3d::StaticInitialize(dXCommon->GetDevice(), camera);
	Sprite::StaticInitialize(dXCommon->GetDevice());
	DrawLine::StaticInitialize(dXCommon->GetDevice(), window_width, window_height);
	DrawLine3D::StaticInitialize(dXCommon->GetDevice());
	ParticleManager::Initialize(dXCommon->GetDevice(), dXCommon->GetCmdList());
	LightGroup::StaticInitialize(dXCommon->GetDevice());
	Fbx::StaticInitialize(dXCommon->GetDevice());
	NormalMap::StaticInitialize(dXCommon->GetDevice());
	ComputeShaderManager::StaticInitialize(dXCommon->GetDevice());
	//ゲームシーン初期化
	scene = new GameScene();
	scene->Initialize();

	//デバッグテキストのテクスチャ
	Sprite::LoadTexture(0, L"Resources/LetterResources/debugfont.png");

	//ポストエフェクト初期化
	postEffect = new PostEffect();
	postEffect->Initialize();

	//Audio初期化
	audio = new Audio();

	//深度の初期化
	dXCommon->CreateDepth();
}

bool MainEngine::Update()
{
	input->Update();

	//エスケープか×が押されたときゲーム終了
	if (input->PushKey(DIK_ESCAPE)|| GameFin() == true) { return true; }
	xinput->Update();

	//更新
	scene->Update(audio, camera);

	return false;
}

void MainEngine::DebugNum(float x, float y, float z)
{
	//数字のデバッグ
	swprintf_s(str, L"%f,%f,%f\n", x, y, z);
	OutputDebugString(str);
}

void MainEngine::Draw()
{
	//描画
	postEffect->PreDrawScene(dXCommon->GetCmdList());
	scene->Draw(dXCommon->GetCmdList());
	postEffect->PostDrawScene(dXCommon->GetCmdList());

	//描画前設定
	dXCommon->BeforeDraw();

	//ポストエフェクト描画
	postEffect->Draw(dXCommon->GetCmdList());

	//コマンド実行
	dXCommon->AfterDraw();

	scene->GetConstbufferNum();
}

void MainEngine::FrameRateKeep() {
	// 今の時間を取得
	QueryPerformanceCounter(&timeEnd);
	// (今の時間 - 前フレームの時間) / 周波数 = 経過時間(秒単位)
	frameTime = static_cast<float>(timeEnd.QuadPart - timeStart.QuadPart) / static_cast<float>(timeFreq.QuadPart);

	if (frameTime < MIN_FREAM_TIME) { // 時間に余裕がある
		// ミリ秒に変換
		DWORD sleepTime = static_cast<DWORD>((MIN_FREAM_TIME - frameTime) * 1000);

		timeBeginPeriod(1); // 分解能を上げる(こうしないとSleepの精度はガタガタ)
		Sleep(sleepTime);   // 寝る
		timeEndPeriod(1);   // 戻す

		// 次週に持ち越し(こうしないとfpsが変になる?)
		return;
	}

	if (frameTime > 0.0) { // 経過時間が0より大きい(こうしないと下の計算でゼロ除算になると思われ)
		fps = (fps * 0.99f) + (0.01f / frameTime); // 平均fpsを計算
#ifdef _DEBUG
// デバッグ用(デバッガにFSP出す)
#ifdef UNICODE
		std::wstringstream stream;
#else
		std::stringstream stream;
#endif
		stream << fps << " FPS" << std::endl;
		// カウンタ付けて10回に1回出力、とかにしないと見づらいかもね
		OutputDebugString(stream.str().c_str());
#endif // _DEBUG
	}

	timeStart = timeEnd; // 入れ替え
}

bool MainEngine::GameFin() {
	//×が押されたとき
	if (winApp->Update() == true) {
		return true;
	} else { return false; }

}