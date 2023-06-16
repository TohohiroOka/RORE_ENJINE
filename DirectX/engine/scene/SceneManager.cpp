#include "SceneManager.h"
#include "Scene1.h"
#include "PostEffect.h"
#include "BasePostEffect.h"
#include "JsonLoader.h"

std::unique_ptr<InterfaceScene> SceneManager::scene = nullptr;
InterfaceScene* SceneManager::nextScene = nullptr;

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

void SceneManager::SetPipeline(const std::string& name,std::vector<GraphicsPipelineManager::DrawSet>& _drawSet) {
	std::vector<JsonLoader::Pipeline> addPipe{};
	JsonLoader::LoadPipeline(name, &addPipe);

	const int size = int(addPipe.size());
	_drawSet.resize(size);

	for (int i = 0; i < size; i++) {
		GraphicsPipelineManager::PEPELINE inPepeline{};
		inPepeline.name = addPipe[i].name;
		inPepeline.shaderType = addPipe[i].shaderType;
		inPepeline.inputLayoutType = addPipe[i].inputLayoutType;
		inPepeline.rtvNum = addPipe[i].rtvNum;
		inPepeline.blendMode = addPipe[i].blendMode;
		inPepeline.drawMode = addPipe[i].drawMode;
		inPepeline.drawType = addPipe[i].drawType;
		inPepeline.textureNum = addPipe[i].textureNum;
		inPepeline.rootparams = addPipe[i].rootparams;

		GraphicsPipelineManager::DrawSet addDrawSet{};
		graphicsPipeline->CreatePipeline(name, inPepeline, addDrawSet);
		_drawSet[i] = addDrawSet;
	}
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
	camera->SetEye({ 0,0,10 });

	//サウンド用
	audio = std::make_unique<Audio>();
	//ライト
	light = LightGroup::Create();
	light->SetDirLightActive(0, true);
	light->SetDirLightColor(0, { 1,1,1 });
	light->SetDirLightDir(0, { 0.0f, 0.0f, 1.0f, 0 });

	//最初のシーン設定
	Scene1* firstScene = new Scene1();
	firstScene->Initialize();
	scene = std::unique_ptr<Scene1>(firstScene);
}

void SceneManager::CreatePipeline()
{
	graphicsPipeline = std::make_unique<GraphicsPipelineManager>();

	//OBJ
	{
		std::vector<GraphicsPipelineManager::DrawSet> addDrawSet{};
		SetPipeline("Object3D", addDrawSet);
		Object3d::SetPipeline(addDrawSet);
	}
	//InstanceObject
	{
		std::vector<GraphicsPipelineManager::DrawSet> addDrawSet{};
		SetPipeline("InstanceObject", addDrawSet);
		InstanceObject::SetPipeline(addDrawSet);
	}
	//HEIGHT_MAP
	{
		std::vector<GraphicsPipelineManager::DrawSet> addDrawSet{};
		SetPipeline("HeightMap", addDrawSet);
		HeightMap::SetPipeline(addDrawSet);
	}
	//PrimitiveObject3D
	{
		std::vector<GraphicsPipelineManager::DrawSet> addDrawSet{};
		SetPipeline("PrimitiveObject3D", addDrawSet);
		PrimitiveObject3D::SetPipeline(addDrawSet);
	}
	//SPRITE
	{
		std::vector<GraphicsPipelineManager::DrawSet> addDrawSet{};
		SetPipeline("Sprite", addDrawSet);
		Sprite::SetPipeline(addDrawSet);
	}
	//PARTICLE
	{
		std::vector<GraphicsPipelineManager::DrawSet> addDrawSet{};
		SetPipeline("Particle", addDrawSet);
		ParticleManager::SetPipeline(addDrawSet);
	}
	//POST_EFFECT
	{
		std::vector<GraphicsPipelineManager::DrawSet> addDrawSet{};
		SetPipeline("PostEffect", addDrawSet);
		PostEffect::SetPipeline(addDrawSet);
	}
	//BASE_POST_EFFECT
	{
		std::vector<GraphicsPipelineManager::DrawSet> addDrawSet{};
		SetPipeline("BasePostEffect", addDrawSet);
		BasePostEffect::SetPipeline(addDrawSet);
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

	//シーン更新
	scene->Update();

	//カメラ更新
	camera->Update();

	Base3D::SetCamera(camera.get());
	ParticleManager::SetCamera(camera.get());

	//ライト
	light->Update();
	scene->SetLight(light.get());

	// 3Dオブエクトにライトをセット
	Base3D::SetLightGroup(light.get());
}

void SceneManager::Draw(ID3D12GraphicsCommandList* cmdList)
{
	scene->SetCmdList(cmdList);

	//シーンでのカメラ更新
	scene->CameraUpdate(0, camera.get());

	scene->Draw(0);
}

void SceneManager::ImguiDraw()
{
	scene->ImguiDraw();
}

void SceneManager::GetConstbufferNum()
{
	scene->GetConstbufferNum();
}

void SceneManager::FrameReset()
{
	scene->FrameReset();
}