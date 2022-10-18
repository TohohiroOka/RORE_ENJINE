#include "SceneManager.h"
#include "Boss1.h"
#include "Title.h"
#include "PostEffect.h"

std::unique_ptr<InterfaceScene> SceneManager::scene = nullptr;
InterfaceScene* SceneManager::nextScene = nullptr;

void SetLayout(D3D12_INPUT_ELEMENT_DESC* inputLayout, GraphicsPipelineManager::INPUT_LAYOUT_NUMBER* inputLayoutType,
	int layoutNum)
{
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
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//�@��
		else if (layoutNumber == LAYOUT::NORMAL)
		{
			inputLayout[i] = {
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//2Duv���W
		else if (layoutNumber == LAYOUT::TEXCOORD_2D)
		{
			inputLayout[i] = {
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//3Duv���W
		else if (layoutNumber == LAYOUT::TEXCOORD_3D)
		{
			inputLayout[i] = {
			"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//�{�[���ԍ�
		else if (layoutNumber == LAYOUT::BONEINDICES)
		{
			inputLayout[i] = {
				"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//�{�[���e���x
		else if (layoutNumber == LAYOUT::BONEWEIGHTS)
		{
			inputLayout[i] = {
				"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//�X�P�[��
		else if (layoutNumber == LAYOUT::SCALE)
		{
			inputLayout[i] = {
				"SCALE", 0, DXGI_FORMAT_R32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}
		//�F
		else if (layoutNumber == LAYOUT::COLOR)
		{
			inputLayout[i] = {
				"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
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
	camera = Camera::Create();
	//�T�E���h�p
	audio = std::make_unique<Audio>();
	//���C�g
	light = LightGroup::Create();
	light->SetDirLightActive(0, true);
	light->SetDirLightColor(0, { 1,1,1 });
	light->SetDirLightDir(0, { 0.0f, 0.0f, 1.0f, 0 });

	//�ŏ��̃V�[���ݒ�
	Title* firstScene = new Title();
	firstScene->Initialize();
	scene = std::unique_ptr<Title>(firstScene);
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
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 3;
		inPepeline.rtvNum = 3;

		graphicsPipeline->CreatePipeline("OBJ", inPepeline, inSignature);
		Object3d::SetPipeline(graphicsPipeline->graphicsPipeline["OBJ"]);
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
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 1;
		inPepeline.rtvNum = 1;

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
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 1;
		inPepeline.rtvNum = 3;

		inSignature.object2d = false;
		inSignature.textureNum = 3;
		inSignature.light = true;

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
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 1;
		inPepeline.rtvNum = 1;

		inSignature.object2d = false;
		inSignature.materialData = false;
		inSignature.textureNum = 0;
		inSignature.light = false;

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
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 1;
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
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 1;
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
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 1;
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
		SetLayout(inputLayout, inputLayoutType, arrayNum);
		inPepeline.inputLayout = inputLayout;
		inPepeline.stateNum = 1;
		inPepeline.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		inPepeline.blendMode = GraphicsPipelineManager::BLEND_MODE::ALPHA;
		inPepeline.particl = false;

		inSignature.object2d = true;
		inSignature.textureNum = 4;
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

	//�J�����̃Z�b�g
	InterfaceObject3d::SetCamera(camera.get());
	//Fbx::SetCamera(camera.get());
	DrawLine3D::SetCamera(camera.get());
	//ParticleManager::SetCamera(camera.get());
	scene->SetCamera(camera.get());
	CubeMap::SetCamera(camera.get());
	HeightMap::SetCamera(camera.get());

	//���C�g
	light->Update();
	scene->SetLight(light.get());

	// 3D�I�u�G�N�g�Ƀ��C�g���Z�b�g
	InterfaceObject3d::SetLightGroup(light.get());
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