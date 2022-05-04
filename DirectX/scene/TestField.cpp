#include "TestField.h"
#include "SceneManager.h"
#include "BrowsingCircle.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"

#include <imgui.h>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <memory>

void TestField::Initialize()
{
	//�X�v���C�g
	Sprite::LoadTexture(1, L"Resources/amm.jpg");

	sprite = Sprite::Create(1);

	//���f���ǂݍ���
	uma = Model::CreateFromOBJ("uma");
	ground = Model::CreateFromOBJ("wall");
	block = Model::CreateFromOBJ("Square");

	//�v���C���[����
	PLAYER = Player::Create(uma.get());

	//�n�ʐ���
	GROUND = Ground::Create(ground.get());

	//�G�����I�u�W�F�N�g����
	std::unique_ptr<TouchableObject> Tobject3d = TouchableObject::Create(block.get());
	Tobject3d->SetScale(10.0f);
	Tobject3d->SetPosition({ -100,0,-100 });
	BLOCK = std::move(Tobject3d);

	//NormalMap�e�N�X�`���̓ǂݍ���
	tex[0] = NormalMap::LoadTexture(L"Resources/SubTexture/white1x1.png");
	tex[1] = NormalMap::LoadTexture(L"Resources/wN1.jpg");
	tex[2] = NormalMap::LoadTexture(L"Resources/pN2.png");

	//���I�u�W�F�N�g����
	water = NormalMap::Create();
	water->SetPosition({ 100,5,100 });
	water->SetScale({ 2,2,2 });
	water->SetMainTexColor({ 1.0f,1.0f,1.0f,1.0f });
	water->SetSubTexColor1({ 0.1f, 0.7f, 1.0f, 0.5f });
	water->SetSubTexColor2({ 1.0f, 1.0f, 1.0f, 0.5f });

	//Fbx���f���̓ǂݍ���
	danceModel = FbxModel::Create("uma");
	SpherePBRModel = FbxModel::Create("SpherePBR");
	SpiralPBRModel = FbxModel::Create("SpiralPBR");

	//Fbx���f���I�u�W�F�N�g�̐���
	anm = Fbx::Create(SpiralPBRModel.get());
	anm->SetPosition({ 100,100,0 });
	anm->SetScale({ 15,15,15 });
	//anm->SetOutline(true);
	//anm->SetToon(true);

	//�p�[�e�B�N���p�e�N�X�`���̓ǂݍ���
	ParticleManager::LoadTexture(0, L"Resources/particle/effect1.png");

	emit = Emitter::Create(0);

	//��
	line = DrawLine::Create();
	line_t = DrawLine::Create();
	line3d = DrawLine3D::Create(10);

	//�R���s���[�g�V�F�[�_�[����
	compute = ComputeShaderManager::Create();
	for (int i = 0; i < max; i++)
	{
		startPosition[i] = { (float)10 * i,(float)10,(float)10 };//�������W
		endPosition[i] = { (float)10,(float)10,(float)10 * i };//�I�����W
		nowPosition[i] = startPosition[i];//���ݍ��W
		time[i] = 0;
	}
}

void TestField::Update()
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//�V�[���̈ڍs
	if (input->TriggerKey(DIK_0))
	{
		BrowsingCircle* nextScene = new BrowsingCircle();
		nextScene->Initialize();
		SceneManager::SetNextScene(nextScene);
	}

	if (input->PushKey(DIK_LEFT)) { cameraAngle++; }
	if (input->PushKey(DIK_RIGHT)) { cameraAngle--; }
	if (input->PushKey(DIK_UP)) { cameraY++; }
	if (input->PushKey(DIK_DOWN)) { cameraY--; }

	//Obj
	PLAYER->SetCameraAngle(cameraAngle);
	PLAYER->Update();
	GROUND->Update();
	BLOCK->Update();

	//NormalMap
	XMFLOAT4 normal = { 1.0f,1.0f,1.0f,1.0f };
	//�@���}�b�v�̈ړ�����
	if (lightF) { lightPos -= 0.5f; } else if (lightF == false) { lightPos += 0.5f; }

	if (lightPos > 30) { lightF = true; }
	if (lightPos < -30) { lightF = false; }

	uvPos += 0.2f;
	water->SetUvPosition(uvPos);
	water->SetLightPosition(lightPos);
	water->Update();

	//Fbx
	anm->Update();

	//�p�[�e�B�N��
	emit->InEmitter(50, 30, { (float)(rand() % 5) - 100,0,(float)(rand() % 5) }, { 0,2,0 },
		{ 0,0,0 }, 50, 1, { 1,1,1,1 }, { 0,0,0,1 });

	//�X�v���C�g
	sprite->Update();
	emit->Update();

	////��
	//line->SetLine({ 300,300 }, { 700,700 }, { 1,1,1,1 }, 50);
	//line->Update();
	//line_t->SetLine({ 20,500 }, { 500,400 }, { 1,1,1,1 }, 50);
	//line_t->Update();
	//XMFLOAT3 S_pos[10];
	//XMFLOAT3 E_pos[10];
	//for (int i = 0; i < 10; i++)
	//{
	//	S_pos[i] = { 0,(float)20 * i,(float)i * 10 };
	//	E_pos[i] = { 0,(float)20 * i,(float)-i * 10 };
	//}
	//line3d->SetLine(S_pos, E_pos, 50);
	//line3d->SetColor({ 1,1,1,1 });
	//line3d->Update();

	//�J�����X�V
	float cameraRadius = DirectX::XMConvertToRadians(cameraAngle);
	const float range = 50.0f;
	XMFLOAT3 cameraPos = PLAYER->GetPosition();
	cameraPos.y += 40.0f;
	camera->SetEye(cameraPos);
	camera->SetTarget({ cosf(cameraRadius) * range + cameraPos.x,cameraY,sinf(cameraRadius) * range + cameraPos.z });

	input = nullptr;
	xinput = nullptr;
}

void TestField::Draw()
{
	assert(cmdList);

	NormalMap::PreDraw(cmdList);
	water->Draw(tex[0], tex[1], tex[2]);
	NormalMap::PostDraw();

	Object3d::PreDraw(cmdList);
	PLAYER->Draw();
	GROUND->Draw();
	BLOCK->Draw();
	Object3d::PostDraw();

	Fbx::PreDraw(cmdList);
	anm->Draw();
	Fbx::PostDraw();

	//�X�v���C�g�`��
	Sprite::PreDraw(cmdList);
	sprite->Draw();
	DebugText::GetInstance()->DrawAll(cmdList);
	Sprite::PostDraw();

	//��
	DrawLine::PreDraw(cmdList);
	line->Draw();
	line_t->Draw();
	DrawLine::PostDraw();

	//��3d
	DrawLine3D::PreDraw(cmdList);
	line3d->Draw();
	DrawLine3D::PostDraw();

	ParticleManager::PreDraw(cmdList);
	//�p�[�e�B�N��
	emit->Draw();
	ParticleManager::PostDraw();

	//�R���s���[�g�V�F�[�_�[
	compute->PreUpdate(cmdList);
	compute->ShaderUpdate(max, startPosition, endPosition, nowPosition, time);
	compute->PostUpdate();
}

void TestField::Finalize()
{
}

void TestField::ImguiDraw()
{
	float baseColor[3];//�x�[�X�J���\
	float metalness;//�����x(0 or 1)
	float specular;//���ʔ��˓x
	float roughness;//�e��

	baseColor[0] = anm->GetBaseColor().x;
	baseColor[1] = anm->GetBaseColor().y;
	baseColor[2] = anm->GetBaseColor().z;
	metalness = anm->GetMetalness();
	specular = anm->GetSpecular();
	roughness = anm->GetRoughness();

	ImGui::Begin("Material");
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(300, 130));
	ImGui::ColorEdit3("baseColor", baseColor, ImGuiColorEditFlags_Float);
	ImGui::SliderFloat("metalness", &metalness, 0, 1);
	ImGui::SliderFloat("specular", &specular, 0, 1);
	ImGui::SliderFloat("roughness", &roughness, 0, 1);
	ImGui::End();

	anm->SetBaseColor({ baseColor[0],baseColor[1],baseColor[2] });
	anm->SetMetalness(metalness);
	anm->SetSpecular(specular);
	anm->SetRoughness(roughness);
}

void TestField::GetConstbufferNum()
{
	XMFLOAT3* inPos = new XMFLOAT3[max];
	inPos = compute->GetConstBufferNum();
	for (int i = 0; i < max; i++)
	{
		nowPosition[i] = inPos[i];
	}
}
