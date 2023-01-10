#include "Boss1.h"
#include "SceneManager.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

using namespace DirectX;

void Boss1::Initialize()
{
	cameraAngle = { -90,50.0f };
	player = Player::Create({0,50,0}, cameraAngle);

	map= HeightMap::Create("heightmap02.bmp", "jimen.png", "kabe.png");
	map->SetScale({5,5,5});

	bullet = BulletManager::Create();

	// コライダーの追加
	MeshCollider* collider = new MeshCollider;
	map->SetCollider(collider);
	//collider->ConstructTriangles(map->GetModel());
	collider->ConstructTriangles(map->GetHitVertices(), map->GetHitIndices());
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);
}

void Boss1::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	player->Update(cameraAngle);

	if (input->TriggerKey(DIK_RETURN)) {
		houkou++;
		if (houkou >= 5) {
			houkou=1;
		}
	}

	bullet->Update(player->GetPosition());

	map->Update();
	map->AddConstBufferUpdate(1.0f);
}

void Boss1::DrawNotPostB()
{
}

void Boss1::Draw()
{
	InterfaceObject3d::SetCmdList(cmdList);
	HeightMap::PreDraw();
	//map->Draw();

	PrimitiveObject3D::PreDraw();
	map->ColliderDraw();

	Object3d::PreDraw();
	player->Draw();

	InterfaceObject3d::ReleaseCmdList();

	InstanceObject::PreDraw(cmdList);
	bullet->Draw();
	InstanceObject::PostDraw();

}

void Boss1::DrawNotPostA()
{
}

void Boss1::Finalize()
{
}

void Boss1::ImguiDraw()
{
}

void Boss1::CameraUpdate(Camera* camera)
{
	DirectInput* input = DirectInput::GetInstance();
	XInputManager* xinput = XInputManager::GetInstance();

	//カメラ更新
	if (input->PushKey(DIK_LEFT) || xinput->RightStickX(true)) { cameraAngle.x += 3.0f; }
	if (input->PushKey(DIK_RIGHT) || xinput->RightStickX(false)) { cameraAngle.x -= 3.0f; }
	if (input->PushKey(DIK_DOWN) || xinput->RightStickY(false)) {
		cameraAngle.y -= 3.0f;
		//上昇制限
		if (cameraAngle.y < 50.0f) {
			cameraAngle.y = 50.0f;
		}
	}
	if (input->PushKey(DIK_UP) || xinput->RightStickY(true)) {
		cameraAngle.y += 3.0f;
		//下降制限
		if (cameraAngle.y > 110.0f) {
			cameraAngle.y = 110.0f;
		}
	}

	//プレイヤー座標
	XMFLOAT3 playerPos = player->GetPosition();
	const float range = 40.0f;
	XMFLOAT2 cameraRadius = {
		DirectX::XMConvertToRadians(cameraAngle.x),
		DirectX::XMConvertToRadians(cameraAngle.y)
	};

	XMFLOAT3 cameraPos = {
		cosf(cameraRadius.x) * range + playerPos.x,
		cosf(cameraRadius.y) * range + playerPos.y,
		sinf(cameraRadius.x) * range + playerPos.z };

	cameraAngle.x = float(int(cameraAngle.x) % 360);

	camera->SetTarget(playerPos);
	camera->SetEye(cameraPos);
}
