#include "Scene1.h"
#include "SceneManager.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"
#include "WindowApp.h"

using namespace DirectX;

// スクリーン座標をワールド座標に変換
Vector3* Scene1::CalcScreenToWorld(Vector3* _pout, XMFLOAT2 _screenPos, float fz)
{
	XMMATRIX view = DirectX::XMMatrixIdentity();
	XMMATRIX prj = DirectX::XMMatrixIdentity();
	if (camera != nullptr) {
		view = camera->GetView();
		prj = camera->GetProjection();
	}
	XMMATRIX InvView, InvPrj, VP, InvViewport;
	InvView = DirectX::XMMatrixInverse(nullptr, view);
	InvPrj = DirectX::XMMatrixInverse(nullptr, prj);
	VP = DirectX::XMMatrixIdentity();
	XMFLOAT4X4 mtx;
	XMStoreFloat4x4(&mtx, VP);

	XMFLOAT2 screen = { float(WindowApp::GetWindowWidth()),float(WindowApp::GetWindowHeight()) };
	mtx._11 = screen.x / 2.0f;
	mtx._22 = -screen.y / 2.0f;
	mtx._41 = screen.x / 2.0f;
	mtx._42 = screen.y / 2.0f;
	VP = XMLoadFloat4x4(&mtx);
	InvViewport = XMMatrixInverse(nullptr, VP);
	// 逆行列
   // 逆変換
	XMMATRIX tmp = InvViewport * InvPrj * InvView;
	XMVECTOR pout = DirectX::XMVector3TransformCoord(XMVECTOR{ _screenPos.x, _screenPos.y, fz }, tmp);

	*_pout= Vector3{pout.m128_f32[0], pout.m128_f32[1], pout.m128_f32[2]};
	return _pout;
}

void Scene1::Initialize()
{
	camera = nullptr;

	map = Map::Create();
	isDrawLine = false;

	cameraPos = { 0,0,-10 };
	cameraTarget = { 135,0,0 };

	lineObject = std::make_unique<PrimitiveObject3D>();
	lineObject->UpdateWorldMatrix();
}

void Scene1::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	if (input->TriggerKey(DIK_P)) {
		isDrawLine = !isDrawLine;
	}

	if (input->TriggerKey(DIK_O)) {
		map->OutputMap("out", 30);
	}

	XMFLOAT2 mousePos = input->GetMousePoint();
	Vector3 pout = {};
	Vector3 target = {};
	CalcScreenToWorld(&pout, mousePos, 0.0f);
	CalcScreenToWorld(&target, mousePos, 1.0f);

	DebugText* text = DebugText::GetInstance();
	std::string strmX = std::to_string(pout.x);
	std::string strmY = std::to_string(pout.y);
	std::string strmZ = std::to_string(pout.z);
	std::string strmtX = std::to_string(target.x);
	std::string strmtY = std::to_string(target.y);
	std::string strmtZ = std::to_string(target.z);
	text->Print("mause x y z : " + strmX + ":"+ strmY+":"+ strmZ, 100,150);
	text->Print("target  x y z : " + strmtX + ":" + strmtY + ":" + strmtZ, 100, 175);
	text = nullptr;

	map->Update({ pout.x,pout.y,pout.z }, { target.x,target.y,target.z });

	if (input->TriggerMouseButton(DirectInput::MOUSE_BUTTON::MOUSE_LEFT)) {
		map->AddBox();
	}

	//線配列初期化
	lineObject->ResetVertex();
	lineObject->SetVertex({ pout.x,pout.y,pout.z });
	lineObject->SetVertex({ target.x,target.y,target.z });
	lineObject->Initialize();
}

void Scene1::DrawNotPostB()
{
	InstanceObject::PreDraw(cmdList);
	map->InstanceDraw();
	InstanceObject::PostDraw();

	InterfaceObject3d::SetCmdList(cmdList);
	//線
	PrimitiveObject3D::PreDraw();
	if (isDrawLine) {
		map->Draw();
	}
	//lineObject->Draw();
	InterfaceObject3d::ReleaseCmdList();
}

void Scene1::Draw()
{
}

void Scene1::DrawNotPostA()
{
	Sprite::PreDraw(cmdList);
	DebugText::GetInstance()->DrawAll();
	Sprite::PostDraw();
}

void Scene1::Finalize()
{
}

void Scene1::ImguiDraw()
{
}

void Scene1::CameraUpdate(Camera* _camera)
{
	DirectInput* input = DirectInput::GetInstance();

	const float Tgspeed = 1.0f;
	//視界移動
	{
		if (input->PushKey(DIK_RIGHT)) { cameraTarget.x -= Tgspeed; }//右入力
		if (input->PushKey(DIK_LEFT)) { cameraTarget.x += Tgspeed; }//左入力
		if (input->PushKey(DIK_UP)) { cameraTarget.y -= Tgspeed; }//上入力
		if (input->PushKey(DIK_DOWN)) { cameraTarget.y += Tgspeed; }//下入力

		//上下方向の角度制限
		if (cameraTarget.y <= -48) { cameraTarget.y = -48; }//下制限
		if (cameraTarget.y >= 60) { cameraTarget.y = 60; }//上制限

		//横の制限
		if (cameraTarget.x > 360) { cameraTarget.x = 0; }
		if (cameraTarget.x < -360) { cameraTarget.x = 0; }
	}

	//カメラ移動
	float Pspeed = 1.0f;
	XMFLOAT2 radian = { XMConvertToRadians(cameraTarget.x + 90),XMConvertToRadians(cameraTarget.x) };

	//右入力
	if (input->PushKey(DIK_S)) {
		cameraPos.x += Pspeed * cos(radian.x);
		cameraPos.z += Pspeed * sin(radian.x);
	}
	//左入力
	if (input->PushKey(DIK_W)) {
		cameraPos.x -= Pspeed * cos(radian.x);
		cameraPos.z -= Pspeed * sin(radian.x);
	}
	//下入力
	if (input->PushKey(DIK_D)) {
		cameraPos.x -= Pspeed * cos(radian.y);
		cameraPos.z -= Pspeed * sin(radian.y);
	}
	//上入力
	if (input->PushKey(DIK_A)) {
		cameraPos.x += Pspeed * cos(radian.y);
		cameraPos.z += Pspeed * sin(radian.y);
	}
	//下入力
	if (input->PushKey(DIK_E)) {
		cameraPos.y += Pspeed;
	}
	//上入力
	if (input->PushKey(DIK_Z)) {
		cameraPos.y -= Pspeed;
	}

	_camera->SetEye(cameraPos);
	_camera->SetTarget({ cameraPos.x - cosf(radian.x),cameraPos.y - sinf(XMConvertToRadians(cameraTarget.y)) ,cameraPos.z - sinf(radian.x) });

	DebugText* text = DebugText::GetInstance();
	std::string strcameraPosx = std::to_string(cameraPos.x);
	std::string strcameraPosy = std::to_string(cameraPos.y);
	std::string strcameraPosz = std::to_string(cameraPos.z);
	std::string strcameraTargetx = std::to_string(cameraTarget.x);
	std::string strcameraTargety = std::to_string(cameraTarget.y);
	std::string strcameraTargetz = std::to_string(cameraTarget.z);
	text->Print("cameraPos x y z : " + strcameraPosx + ":" + strcameraPosy + ":" + strcameraPosz, 100, 100);
	text->Print("cameraTarget x y z : " + strcameraTargetx + ":" + strcameraTargety + ":" + strcameraTargetz, 100, 125);
	text->Print("cameraAngle: " + std::to_string(cameraTarget.x), 100, 200);

	text = nullptr;

	camera = _camera;
}