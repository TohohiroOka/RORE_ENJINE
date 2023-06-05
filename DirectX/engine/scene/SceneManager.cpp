#include "SceneManager.h"
#include "Scene1.h"
#include "PostEffect.h"

std::unique_ptr<InterfaceScene> SceneManager::scene = nullptr;
InterfaceScene* SceneManager::nextScene = nullptr;

void SetLayout(D3D12_INPUT_ELEMENT_DESC* inputLayout, GraphicsPipelineManager::INPUT_LAYOUT_NUMBER* inputLayoutType,
	int layoutNum, bool instance)
{
	D3D12_INPUT_CLASSIFICATION input = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	if (instance)
	{
		input = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
	}
	using LAYOUT = GraphicsPipelineManager::INPUT_LAYOUT_NUMBER;
	// ���_���C�A�E�g�̐ݒ�
	for (int i = 0; i < layoutNum; i++)
	{
		int layoutNumber = inputLayoutType[i];
		//���W
		if (layoutNumber == LAYOUT::POSITION)
		{
			inputLayout[i] = {
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//�@��
		else if (layoutNumber == LAYOUT::NORMAL)
		{
			inputLayout[i] = {
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//2Duv���W
		else if (layoutNumber == LAYOUT::TEXCOORD_2D)
		{
			inputLayout[i] = {
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//3Duv���W
		else if (layoutNumber == LAYOUT::TEXCOORD_3D)
		{
			inputLayout[i] = {
			"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//�{�[���ԍ�
		else if (layoutNumber == LAYOUT::BONEINDICES)
		{
			inputLayout[i] = {
				"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//�{�[���e���x
		else if (layoutNumber == LAYOUT::BONEWEIGHTS)
		{
			inputLayout[i] = {
				"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//�X�P�[��
		else if (layoutNumber == LAYOUT::SCALE)
		{
			inputLayout[i] = {
				"SCALE", 0, DXGI_FORMAT_R32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
		//�F
		else if (layoutNumber == LAYOUT::COLOR)
		{
			inputLayout[i] = {
				"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,input, 0 };
		}
	}
}

std::unique_ptr<SceneManager> SceneManager::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	SceneManager* instance = new SceneManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
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

	//�J�����̏�����
	camera = Camera::Create(0);
	camera->SetEye({ 0,0,10 });

	//�T�E���h�p
	audio = std::make_unique<Audio>();
	//���C�g
	light = LightGroup::Create();
	light->SetDirLightActive(0, true);
	light->SetDirLightColor(0, { 1,1,1 });
	light->SetDirLightDir(0, { 0.0f, 0.0f, 1.0f, 0 });

	//�ŏ��̃V�[���ݒ�
	Scene1* firstScene = new Scene1();
	firstScene->Initialize();
	scene = std::unique_ptr<Scene1>(firstScene);
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

		//�z��T�C�Y
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum, false);
		inPepeline.inputLayout = inputLayout;
		inPepeline.rtvNum = 1;

		graphicsPipeline->CreatePipeline("OBJ", inPepeline, inSignature);
		Object3d::SetPipeline(graphicsPipeline->graphicsPipeline["OBJ"]);
	}
	//InstanceObject
	{
		inPepeline.object2d = false;
		inPepeline.vertShader = "InstanceObject";
		inPepeline.pixelShader = "InstanceObject";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION ,GraphicsPipelineManager::NORMAL,GraphicsPipelineManager::TEXCOORD_2D };

		//�z��T�C�Y
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum, false);
		inPepeline.inputLayout = inputLayout;
		inPepeline.rtvNum = 1;
		inPepeline.blendMode = GraphicsPipelineManager::BLEND_MODE::ALPHA;

		inSignature.instanceDraw = true;

		graphicsPipeline->CreatePipeline("InstanceObject", inPepeline, inSignature);
		InstanceObject::SetPipeline(graphicsPipeline->graphicsPipeline["InstanceObject"]);
	}
	//CUBE_BOX
	{
		inPepeline.object2d = false;
		inPepeline.vertShader = "CUBE_BOX";
		inPepeline.pixelShader = "CUBE_BOX";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION ,GraphicsPipelineManager::TEXCOORD_3D };
		//�z��T�C�Y
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum, false);
		inPepeline.inputLayout = inputLayout;
		inPepeline.rtvNum = 1;

		inSignature.instanceDraw = false;

		graphicsPipeline->CreatePipeline("CUBE_BOX", inPepeline, inSignature);
		CubeMap::SetPipeline(graphicsPipeline->graphicsPipeline["CUBE_BOX"]);
	}
	//HEIGHT_MAP
	{
		inPepeline.object2d = false;
		inPepeline.vertShader = "HEIGHT_MAP";
		inPepeline.pixelShader = "HEIGHT_MAP";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION ,GraphicsPipelineManager::NORMAL,GraphicsPipelineManager::TEXCOORD_2D };
		//�z��T�C�Y
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum, false);
		inPepeline.inputLayout = inputLayout;
		inPepeline.rtvNum = 1;
		inPepeline.blendMode = GraphicsPipelineManager::BLEND_MODE::ALPHA;

		inSignature.object2d = false;
		inSignature.textureNum = 3;
		inSignature.light = true;
		inSignature.instanceDraw = true;//�萔�o�b�t�@�Ƃ��ăC���X�^���X�`��p�̕���p����

		graphicsPipeline->CreatePipeline("HEIGHT_MAP", inPepeline, inSignature);
		HeightMap::SetPipeline(graphicsPipeline->graphicsPipeline["HEIGHT_MAP"]);
	}
	//DRAW_LINE_3D
	{
		inPepeline.object2d = false;
		inPepeline.vertShader = "DRAW_LINE_3D";
		inPepeline.pixelShader = "DRAW_LINE_3D";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION };
		//�z��T�C�Y
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum, false);
		inPepeline.inputLayout = inputLayout;
		inPepeline.rtvNum = 1;

		inSignature.object2d = false;
		inSignature.materialData = false;
		inSignature.textureNum = 0;
		inSignature.light = false;
		inSignature.instanceDraw = false;

		graphicsPipeline->CreatePipeline("DRAW_LINE_3D", inPepeline, inSignature);
		DrawLine3D::SetPipeline(graphicsPipeline->graphicsPipeline["DRAW_LINE_3D"]);
	}
	//PrimitiveObject3D
	{
		inPepeline.object2d = false;
		inPepeline.vertShader = "DRAW_LINE_3D";
		inPepeline.pixelShader = "DRAW_LINE_3D";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION };
		//�z��T�C�Y
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum, false);
		inPepeline.inputLayout = inputLayout;
		inPepeline.rtvNum = 1;
		inPepeline.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

		inSignature.object2d = false;
		inSignature.materialData = false;
		inSignature.textureNum = 0;
		inSignature.light = false;

		graphicsPipeline->CreatePipeline("PrimitiveObject3D", inPepeline, inSignature);
		PrimitiveObject3D::SetPipeline(graphicsPipeline->graphicsPipeline["PrimitiveObject3D"]);
	}
	//SPRITE
	{
		inPepeline.object2d = true;
		inPepeline.vertShader = "SPRITE";
		inPepeline.pixelShader = "SPRITE";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION ,GraphicsPipelineManager::TEXCOORD_2D };
		//�z��T�C�Y
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum, false);
		inPepeline.inputLayout = inputLayout;
		inPepeline.rtvNum = 1;
		inPepeline.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		inSignature.object2d = true;
		inSignature.textureNum = 1;
		inSignature.light = false;

		graphicsPipeline->CreatePipeline("SPRITE", inPepeline, inSignature);
		Sprite::SetPipeline(graphicsPipeline->graphicsPipeline["SPRITE"]);
	}
	//PARTICLE
	{
		inPepeline.object2d = true;
		inPepeline.vertShader = "PARTICLE";
		inPepeline.pixelShader = "PARTICLE";
		inPepeline.geometryShader = "PARTICLE";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION ,GraphicsPipelineManager::SCALE,GraphicsPipelineManager::COLOR };
		//�z��T�C�Y
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum, false);
		inPepeline.inputLayout = inputLayout;
		inPepeline.rtvNum = 1;
		inPepeline.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		inPepeline.blendMode = GraphicsPipelineManager::BLEND_MODE::ADD;
		inPepeline.particl = true;

		inSignature.object2d = true;
		inSignature.textureNum = 1;
		inSignature.light = false;

		graphicsPipeline->CreatePipeline("PARTICLE", inPepeline, inSignature);
		ParticleManager::SetPipeline(graphicsPipeline->graphicsPipeline["PARTICLE"]);
	}
	//POST_EFFECT
	{
		inPepeline.object2d = true;
		inPepeline.vertShader = "POST_EFFECT";
		inPepeline.pixelShader = "POST_EFFECT";
		inPepeline.geometryShader = "null";
		GraphicsPipelineManager::INPUT_LAYOUT_NUMBER inputLayoutType[] = {
			GraphicsPipelineManager::POSITION ,GraphicsPipelineManager::TEXCOORD_2D };
		//�z��T�C�Y
		const int arrayNum = sizeof(inputLayoutType) / sizeof(inputLayoutType[0]);

		inPepeline.layoutNum = arrayNum;
		D3D12_INPUT_ELEMENT_DESC inputLayout[arrayNum];
		SetLayout(inputLayout, inputLayoutType, arrayNum, false);
		inPepeline.inputLayout = inputLayout;
		inPepeline.rtvNum = 1;
		inPepeline.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		inPepeline.blendMode = GraphicsPipelineManager::BLEND_MODE::ALPHA;
		inPepeline.particl = false;
		
		inSignature.object2d = true;
		inSignature.textureNum = 1;
		inSignature.light = false;
		graphicsPipeline->CreatePipeline("POST_EFFECT", inPepeline, inSignature);
		PostEffect::SetPipeline(graphicsPipeline->graphicsPipeline["POST_EFFECT"]);
	}

}

void SceneManager::Update()
{
	//�V�[���؂�ւ�
	if (nextScene)
	{
		if (scene)
		{
			scene.reset();
			Sprite::SceneFinalize();
			//ParticleManager::SceneFinalize();
		}

		//�V�[���؂�ւ�
		scene = std::unique_ptr<InterfaceScene>(nextScene);
		nextScene = nullptr;

		//������
		scene->Initialize();
	}

	//�V�[���X�V
	scene->Update();

	//�J�����X�V
	camera->Update();

	InterfaceObject3d::SetCamera(camera.get());
	InstanceObject::SetCamera(camera.get());
	DrawLine3D::SetCamera(camera.get());
	ParticleManager::SetCamera(camera.get());
	CubeMap::SetCamera(camera.get());
	HeightMap::SetCamera(camera.get());

	//���C�g
	light->Update();
	scene->SetLight(light.get());

	// 3D�I�u�G�N�g�Ƀ��C�g���Z�b�g
	InstanceObject::SetLightGroup(light.get());
	InterfaceObject3d::SetLightGroup(light.get());
	HeightMap::SetLightGroup(light.get());
}

void SceneManager::Draw(ID3D12GraphicsCommandList* cmdList)
{
	scene->SetCmdList(cmdList);

	//�V�[���ł̃J�����X�V
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