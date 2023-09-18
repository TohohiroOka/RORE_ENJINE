#include "Scene1.h"
#include "Scene/SceneManager.h"
#include "Input/DirectInput.h"
#include "Input/XInputManager.h"
#include "Object/2d/DebugText.h"
#include "WindowApp.h"
#include <imgui.h>
#include "Object/3d/collider/Collision.h"
#include "Object/3d/collider/MeshCollider.h"
#include "Object/3d/collider/CollisionAttribute.h"

using namespace DirectX;

const std::array<XMFLOAT4, 2> COLOR = { XMFLOAT4{ 0.0f,0.0f,0.8f,1.0f } ,{ 0.8f,0.0f,0.0f,1.0f } };

void Scene1::Initialize()
{
	camera = nullptr;

	Sprite::LoadTexture("amm","Resources/amm.jpg");
	sprite = Sprite::Create("amm");
	sprite->SetTexSize({ 1059.0f,1500.0f });
	sprite->SetSize({ 1059.0f / 5.0f,1500.0f / 5.0f });
	sprite->Update();

	const std::string jimen = "jimen.png";
	const std::string kabe = "kabe.png";

	m_model = TerrainModel::Create("heightmap02.bmp", 5.0f, TerrainModel::FACE_DIRECTION::Y_PLUS,
		{ 0,0,0 }, 5.0f, jimen, kabe);

	object = HeightMap::Create(m_model.get());
	object->SetScale({ 5.0f ,5.0f ,5.0f });

	//object->SetLight(false);
	//元の判定消去
	object->DeleteCollider();

	// コライダーの追加
	MeshCollider* collider = new MeshCollider;
	object->SetCollider(collider);
	collider->ConstructTriangles(object->GetModel());
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	player = std::make_unique<Player>();
}

void Scene1::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	player->Update();
	object->Update();
}

void Scene1::Draw(const int _cameraNum)
{
	//object->Draw();

	object->ColliderDraw();

	player->Draw();
}

void Scene1::NonPostEffectDraw(const int _cameraNum)
{
	//スプライト
	if (_cameraNum == 0) {
		DebugText::GetInstance()->DrawAll();
		sprite->Draw();
	}
}

void Scene1::Finalize()
{
}

void Scene1::ImguiDraw()
{
	ImGui::Begin("debug imgui");
	ImGui::SetWindowSize(ImVec2(300, 300), ImGuiCond_::ImGuiCond_FirstUseEver);

	ImGui::Text("Map Change Direction");
	ImGui::End();
}

void Scene1::CameraUpdate(const int _cameraNum, Camera* _camera)
{
	const float camera_sa = 10.0f;
	const float LR_sa = 163.0f;
	const float UD_sa = 53.0f;

	//通常カメラ
	if (_cameraNum == 0) {
		DirectInput* input = DirectInput::GetInstance();
		//視界移動
		const float Tgspeed = 1.0f;
		if (input->PushKey(DIK_RIGHT)) { cameraTarget.x -= Tgspeed; }//右入力
		if (input->PushKey(DIK_LEFT)) { cameraTarget.x += Tgspeed; }//左入力
		if (input->PushKey(DIK_DOWN)) { cameraTarget.y -= 0.01f + abs(cameraTarget.y) * 0.01f; }//下入力
		if (input->PushKey(DIK_UP)) { cameraTarget.y += 0.01f + abs(cameraTarget.y) * 0.01f; }//上入力

		//上下方向の角度制限
		if (cameraTarget.y <= -13) { cameraTarget.y = -13; }//下制限
		if (cameraTarget.y >= 13) { cameraTarget.y = 13; }//上制限

		//横の制限
		if (cameraTarget.x > 360) { cameraTarget.x = 0; }
		if (cameraTarget.x < -360) { cameraTarget.x = 0; }

		//カメラ移動
		float Pspeed = 1.0f;
		XMFLOAT2 radian = { XMConvertToRadians(cameraTarget.x + 90),XMConvertToRadians(cameraTarget.x) };

		////右入力
		//if (input->PushKey(DIK_S)) {
		//	cameraPos.x += Pspeed * cos(radian.x);
		//	cameraPos.z += Pspeed * sin(radian.x);
		//}
		////左入力
		//if (input->PushKey(DIK_W)) {
		//	cameraPos.x -= Pspeed * cos(radian.x);
		//	cameraPos.z -= Pspeed * sin(radian.x);
		//}
		////下入力
		//if (input->PushKey(DIK_D)) {
		//	cameraPos.x -= Pspeed * cos(radian.y);
		//	cameraPos.z -= Pspeed * sin(radian.y);
		//}
		////上入力
		//if (input->PushKey(DIK_A)) {
		//	cameraPos.x += Pspeed * cos(radian.y);
		//	cameraPos.z += Pspeed * sin(radian.y);
		//}
		////下入力
		//if (input->PushKey(DIK_SPACE)) {
		//	cameraPos.y += Pspeed;
		//}
		////上入力
		//if (input->PushKey(DIK_LSHIFT)) {
		//	cameraPos.y -= Pspeed;
		//}

		Vector3 ppos= player->GetPos();

		_camera->SetEye({ ppos.x,ppos.y + 10.0f,ppos.z - 20.0f });
		_camera->SetTarget(ppos);

		camera = _camera;
	}
}

void Scene1::FrameReset()
{
}