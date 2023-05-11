#include "Scene1.h"
#include "SceneManager.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"
#include "WindowApp.h"
#include <imgui.h>

using namespace DirectX;

char Scene1::fileName[36] = "";

const std::array<XMFLOAT4,2> COLOR={ XMFLOAT4{ 0.0f,0.0f,0.8f,1.0f } ,{ 0.8f,0.0f,0.0f,1.0f } };

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
	isDrawLine = true;
	isSetObject = { false,false,true };

	cameraPos = { 0,0,-10 };
	cameraTarget = { 135,0,0 };

	isDelete = false;

	imguiPos = { 0,0 };
	kaburi = false;

	exportTimer = 100;
	improtTimer = 100;
	isImprot = false;

	imguiColor = COLOR[0];

	isOutsideCollision = true;
}

void Scene1::Update()
{
	DirectInput* input = DirectInput::GetInstance();

	//フレームの最初に初期化するもの
	kaburi = false;
	if (mapChange) {
		map->ChangeDelimitNum(mapChangeSize);
		mapChange = false;
	}
	if (isImprot) {
		if (!map->ImputMap(fileName)) {
			imguiColor = COLOR[1];
		}
		improtTimer = 0;
		isImprot = false;
	}
	//マウス座標
	XMFLOAT2 mousePos = input->GetMousePoint();
	Vector3 pout = {};
	Vector3 target = {};
	CalcScreenToWorld(&pout, mousePos, 0.0f);
	CalcScreenToWorld(&target, mousePos, 1.0f);

	if (imguiPos.x < mousePos.x && imguiPos.x + imguiMax.x > mousePos.x &&
		imguiPos.y < mousePos.y && imguiPos.y + imguiMax.y > mousePos.y)
	{
		kaburi = true;
	}

	if (!kaburi) {
		//オブジェクト設置
		if (input->TriggerMouseButton(DirectInput::MOUSE_BUTTON::MOUSE_LEFT) && frame) {
			if (!isDelete) {
				map->AddBox(camera->GetEye(), isSetObject);
			} else {
				map->DeleteBox(camera->GetEye());
			}
			frame = false;
		}
	}

	if (input->TriggerKey(DIK_1)) {
		isSetObject[0] = true;
		isSetObject[1] = false;
		isSetObject[2] = false;
	}
	if (input->TriggerKey(DIK_2)) {
		isSetObject[0] = false;
		isSetObject[1] = true;
		isSetObject[2] = false;
	}
	if (input->TriggerKey(DIK_3)) {
		isSetObject[0] = false;
		isSetObject[1] = false;
		isSetObject[2] = true;
	}
	if (input->TriggerKey(DIK_4)) {
		isDrawLine = !isDrawLine;
	}
	if (input->TriggerKey(DIK_5)) {
		isDelete = !isDelete;
	}
	if (input->TriggerKey(DIK_6)) {
		isOutsideCollision = !isOutsideCollision;
	}

	map->Update(pout, target, kaburi, isOutsideCollision);

	DebugText* text = DebugText::GetInstance();
	if (exportTimer != 100) {
		exportTimer++;
		text->Print("export", 100, 100, { 1.0f,1.0f ,1.0f }, 20);
		if (exportTimer == 20) { exportTimer = 100; }
	}
	if (improtTimer != 100) {
		improtTimer++;
		if (improtTimer == 40) {
			exportTimer = 100;
			imguiColor = COLOR[0];
		}
	}
	text = nullptr;
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
	InterfaceObject3d::ReleaseCmdList();

	frame = true;
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
	DirectInput* input = DirectInput::GetInstance();

	//マップサイズ
	const XMINT3 mapSize = map->GetDelimitNum();
	mapChangeSize = mapSize;

	std::array<bool, 3> isSetFlag = isSetObject;

	ImGui::Begin("MapEditor");
	//ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(imguiColor.x, imguiColor.y, imguiColor.z, imguiColor.w));
	ImGui::SetWindowPos(ImVec2(imguiPos.x, imguiPos.y));
	ImGui::SetWindowSize(ImVec2(imguiMax.x, imguiMax.y));

	ImGui::InputText("FileName", fileName, sizeof(fileName));
	if (ImGui::Button("export [0]")|| input->TriggerKey(DIK_0)) {
		map->OutputMap(fileName, 30);
		exportTimer = 0;
	}
	if (ImGui::Button("import [9]") || input->TriggerKey(DIK_9)) {
		isImprot = true;
	}
	ImGui::SliderInt("MapSize : X", &mapChangeSize.x, 1, 20);
	ImGui::SliderInt("MapSize : Y", &mapChangeSize.y, 1, 20);
	ImGui::SliderInt("MapSize : Z", &mapChangeSize.z, 1, 20);
	ImGui::Checkbox("Player [1]", &isSetObject[0]);
	ImGui::Checkbox("Goal [2]", &isSetObject[1]);
	ImGui::Checkbox("NormalObject [3]", &isSetObject[2]);
	ImGui::Checkbox("Line Draw [4]", &isDrawLine);
	ImGui::Checkbox("Box Delete [5]", &isDelete);
	ImGui::Checkbox("Outside collision [6]", &isOutsideCollision);

	ImGui::End();

	//設置フラグ管理
	if (isSetFlag[0] != isSetObject[0] && isSetObject[0]) {
		isSetObject[1] = false;
		isSetObject[2] = false;
	} else if (isSetFlag[1] != isSetObject[1] && isSetObject[1]) {
		isSetObject[0] = false;
		isSetObject[2] = false;
	} else if (isSetFlag[2] != isSetObject[2] && isSetObject[2]) {
		isSetObject[0] = false;
		isSetObject[1] = false;
	}

	//サイズ変更
	if (mapChangeSize.x != mapSize.x || mapChangeSize.y != mapSize.y || mapChangeSize.z != mapSize.z) {
		mapChange = true;
	}

	input = nullptr;
}

void Scene1::CameraUpdate(Camera* _camera)
{
	DirectInput* input = DirectInput::GetInstance();
	//視界移動
	if (!kaburi) {
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
	}

	//カメラ移動
	float Pspeed = 1.0f;
	XMFLOAT2 radian = { XMConvertToRadians(cameraTarget.x + 90),XMConvertToRadians(cameraTarget.x) };

	if (!kaburi) {
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
		if (input->PushKey(DIK_SPACE)) {
			cameraPos.y += Pspeed;
		}
		//上入力
		if (input->PushKey(DIK_LSHIFT)) {
			cameraPos.y -= Pspeed;
		}
	}

	_camera->SetEye(cameraPos);
	_camera->SetTarget({ cameraPos.x - cosf(radian.x),cameraPos.y + cameraTarget.y ,cameraPos.z - sinf(radian.x) });

	camera = _camera;
}