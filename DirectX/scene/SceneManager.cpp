#include "SceneManager.h"
#include "TestField.h"
#include "PostEffect.h"

std::unique_ptr<InterfaceScene> SceneManager::scene = nullptr;
InterfaceScene* SceneManager::nextScene = nullptr;

void SetLayout(D3D12_INPUT_ELEMENT_DESC* inputLayout, GraphicsPipelineManager::INPUT_LAYOUT_NUMBER* inputLayoutType,
	int layoutNum)
{
	using LAYOUT = GraphicsPipelineManager::INPUT_LAYOUT_NUMBER;
	// 頂点レイアウトの設定
	for (int i = 0; i < layoutNum; i++)
	{
		int layoutNumber = inputLayoutType[i];
		//座標
		if (layoutNumber == LAYOUT::POSITION)
		{
			inputLayout[i] = {
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//法線
		else if (layoutNumber == LAYOUT::NORMAL)
		{
			inputLayout[i] = {
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//2Duv座標
		else if (layoutNumber == LAYOUT::TEXCOORD_2D)
		{
			inputLayout[i] = {
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//3Duv座標
		else if (layoutNumber == LAYOUT::TEXCOORD_3D)
		{
			inputLayout[i] = {
			"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//ボーン番号
		else if (layoutNumber == LAYOUT::BONEINDICES)
		{
			inputLayout[i] = {
				"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//ボーン影響度
		else if (layoutNumber == LAYOUT::BONEWEIGHTS)
		{
			inputLayout[i] = {
				"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
	}
}

std::unique_ptr<SceneManager> SceneManager::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	SceneManager* instance = new SceneManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();

	return std::unique_ptr<SceneManager>(instance);
}

SceneManager::~SceneManager()
{
	scene.reset();
}

void SceneManager::Initialize()
{
	CreatePipeline();
	
	//カメラの初期化
	camera = Camera::Create();
	//サウンド用
	audio = std::make_unique<Audio>();
	//ライト
	light = LightGroup::Create();
	light->SetDirLightActive(0, true);
	light->SetDirLightColor(0, { 1,1,1 });
	light->SetDirLightDir(0, { 0.0f, 0.0f, 1.0f, 0 });

	//最初のシーン設定
	TestField* firstScene = new TestField();
	firstScene->Initialize();
	scene = std::unique_ptr<TestField>(firstScene);
}

void SceneManager::CreatePipeline()
{
	graphicsPipeline = std::make_unique<GraphicsPipelineManager>();

	GraphicsPipelineManager::PEPELINE_DESC inPepeline{};
	GraphicsPipelineManager::SIGNATURE_DESC inSignature{};

	//OBJ
	{
		inPepeline.object2d = false;
		inPepeline.vertShader = "OBJ";
		inPepeline.pixelShader = "OBJ";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION ,GraphicsPipelineManager::NORMAL,GraphicsPipelineManager::TEXCOORD_2D };

		//配列サイズ
		const int arrayNum= sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 3;

		graphicsPipeline->CreatePipeline("OBJ", &inPepeline, &inSignature);
		Object3d::SetPipe(graphicsPipeline->graphicsPipeline["OBJ"]);
	}
	//CUBE_BOX
	{
		inPepeline.object2d = false;
		inPepeline.vertShader = "CUBE_BOX";
		inPepeline.pixelShader = "CUBE_BOX";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION ,GraphicsPipelineManager::TEXCOORD_3D };
		//配列サイズ
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 1;

		graphicsPipeline->CreatePipeline("CUBE_BOX", &inPepeline, &inSignature);
		CubeMap::SetPipe(graphicsPipeline->graphicsPipeline["CUBE_BOX"]);
	}
	//HEIGHT_MAP
	{
		inPepeline.object2d = false;
		inPepeline.vertShader = "HEIGHT_MAP";
		inPepeline.pixelShader = "HEIGHT_MAP";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION ,GraphicsPipelineManager::NORMAL,GraphicsPipelineManager::TEXCOORD_2D };
		//配列サイズ
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 1;

		inSignature.object2d = false;
		inSignature.textureNum = 2;
		inSignature.light = true;

		graphicsPipeline->CreatePipeline("HEIGHT_MAP", &inPepeline, &inSignature);
		HeightMap::SetPipe(graphicsPipeline->graphicsPipeline["HEIGHT_MAP"]);
	}
	//SPRITE
	{
		inPepeline.object2d = true;
		inPepeline.vertShader = "SPRITE";
		inPepeline.pixelShader = "SPRITE";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION ,GraphicsPipelineManager::TEXCOORD_2D };
		//配列サイズ
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 1;

		inSignature.object2d = true;
		inSignature.textureNum = 1;
		inSignature.light = false;

		graphicsPipeline->CreatePipeline("SPRITE", &inPepeline, &inSignature);
		Sprite::SetPipe(graphicsPipeline->graphicsPipeline["SPRITE"]);
	}
	//POST_EFFECT
	{
		inPepeline.object2d = true;
		inPepeline.vertShader = "POST_EFFECT";
		inPepeline.pixelShader = "POST_EFFECT";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION ,GraphicsPipelineManager::TEXCOORD_2D };
		//配列サイズ
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 1;

		inSignature.object2d = true;
		inSignature.light = false;
		inSignature.textureNum = 2;

		graphicsPipeline->CreatePipeline("POST_EFFECT", &inPepeline, &inSignature);
		PostEffect::SetPipe(graphicsPipeline->graphicsPipeline["POST_EFFECT"]);
	}
}

void SceneManager::Update()
{
	//シーン切り替え
	if (nextScene)
	{
		if (scene)
		{
			scene.reset();
			Sprite::SceneFinalize();
			//ParticleManager::SceneFinalize();
		}

		//シーン切り替え
		scene = std::unique_ptr<InterfaceScene>(nextScene);
		nextScene = nullptr;

		//初期化
		scene->Initialize();
	}

	//カメラのセット
	Object3d::SetCamera(camera.get());
	//Fbx::SetCamera(camera.get());
	//DrawLine3D::SetCamera(camera.get());
	//ParticleManager::SetCamera(camera.get());
	scene->SetCamera(camera.get());
	CubeMap::SetCamera(camera.get());
	HeightMap::SetCamera(camera.get());

	//ライト
	light->Update();
	scene->SetLight(light.get());

	// 3Dオブエクトにライトをセット
	Object3d::SetLightGroup(light.get());
	//Fbx::SetLightGroup(light.get());
	HeightMap::SetLightGroup(light.get());

	camera->Update();
	scene->Update();
}

void SceneManager::Draw(ID3D12GraphicsCommandList* cmdList)
{
	scene->SetCmdList(cmdList);
	scene->Draw();
}

void SceneManager::ImguiDraw()
{
	scene->ImguiDraw();
}

void SceneManager::GetConstbufferNum()
{
	scene->GetConstbufferNum();
}