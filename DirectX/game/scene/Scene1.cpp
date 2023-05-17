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

	if (!kaburi) {
		//オブジェクト設置
		if (input->TriggerMouseButton(DirectInput::MOUSE_BUTTON::MOUSE_LEFT)) {
			map->AddBox(camera->GetEye(), isSetObject);
		} else if (input->TriggerMouseButton(DirectInput::MOUSE_BUTTON::MOUSE_RIGHT)) {
			map->DeleteBox(camera->GetEye());
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

void Scene1::Draw(const int _cameraNum)
{
	map->SetLight(_cameraNum == 0);

	InterfaceObject3d::SetCmdList(cmdList);
	//線
	PrimitiveObject3D::PreDraw();
	if (isDrawLine) {
		map->Draw();
	}
	InterfaceObject3d::ReleaseCmdList();

	InstanceObject::PreDraw(cmdList);
	map->InstanceDraw();
	InstanceObject::PostDraw();

	if (_cameraNum == 0) {
		Sprite::PreDraw(cmdList);
		DebugText::GetInstance()->DrawAll();
		Sprite::PostDraw();
	}
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
	ImGui::SetWindowSize(ImVec2(imguiMax.x, imguiMax.y), ImGuiCond_::ImGuiCond_FirstUseEver);

	ImGui::InputText("FileName", fileName, sizeof(fileName));
	if (ImGui::Button("export")) {
		map->OutputMap(fileName, 30);
		exportTimer = 0;
	}
	if (ImGui::Button("import")) {
		isImprot = true;
	}
	ImGui::SliderInt("MapSize : X", &mapChangeSize.x, 1, 17);
	ImGui::SliderInt("MapSize : Y", &mapChangeSize.y, 1, 17);
	ImGui::SliderInt("MapSize : Z", &mapChangeSize.z, 1, 17);
	ImGui::Checkbox("Player [1]", &isSetObject[0]);
	ImGui::Checkbox("Goal [2]", &isSetObject[1]);
	ImGui::Checkbox("NormalObject [3]", &isSetObject[2]);
	ImGui::Checkbox("Line Draw [4]", &isDrawLine);
	ImGui::Checkbox("Outside collision [5]", &isOutsideCollision);

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

void Scene1::CameraUpdate(const int _cameraNum, Camera* _camera)
{
	XMINT3 delimitNumInt = map->GetDelimitNum();
	XMFLOAT3 delimitNum = { float(delimitNumInt.x),float(delimitNumInt.y) ,float(delimitNumInt.z) };
	float delimitSize = map->GetDelimitSize();
	const float camera_sa = 10.0f;
	const float LR_sa = 163.0f;
	const float UD_sa = 53.0f;

	//通常カメラ
	if (_cameraNum == 0) {
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
	//後ろ
	else if (_cameraNum == 1) {
		_camera->SetEye({ (delimitNum.x / 2.0f) * delimitSize - LR_sa, (delimitNum.y / 2.0f) * delimitSize - UD_sa ,delimitNum.z * delimitSize + camera_sa });
		_camera->SetTarget({ (delimitNum.x / 2.0f) * delimitSize - LR_sa, (delimitNum.y / 2.0f) * delimitSize - UD_sa ,0.0f });
	}
	//前
	else if (_cameraNum == 2) {
		_camera->SetEye({ (delimitNum.x / 2.0f) * delimitSize + LR_sa,(delimitNum.y / 2.0f) * delimitSize - UD_sa ,-delimitNum.z * delimitSize });
		_camera->SetTarget({ (delimitNum.x / 2.0f) * delimitSize + LR_sa, (delimitNum.y / 2.0f) * delimitSize - UD_sa ,0.0f });
	}
	//上
	else if (_cameraNum == 3) {
		_camera->SetEye({ (delimitNum.x / 2.0f) * delimitSize + LR_sa, delimitNum.y * delimitSize + camera_sa , (delimitNum.z / 2.0f) * delimitSize - UD_sa });
		_camera->SetTarget({ (delimitNum.x / 2.0f) * delimitSize + LR_sa , 0.0f ,(delimitNum.z / 2.0f) * delimitSize - UD_sa });
		_camera->SetUp({ 0.0f,0.0f,0.5f });
	}
	//下
	else if (_cameraNum == 4) {
		_camera->SetEye({ (delimitNum.x / 2.0f) * delimitSize - LR_sa, -delimitNum.y * delimitSize , (delimitNum.z / 2.0f) * delimitSize - UD_sa });
		_camera->SetTarget({ (delimitNum.x / 2.0f) * delimitSize - LR_sa, 0.0f ,(delimitNum.z / 2.0f) * delimitSize - UD_sa });
		_camera->SetUp({ 0.0f,0.0f,0.5f });
	}
	//右
	else if (_cameraNum == 5) {
		_camera->SetEye({ delimitNum.x * delimitSize + camera_sa, (delimitNum.y / 2.0f) * delimitSize - UD_sa , (delimitNum.z / 2.0f) * delimitSize + LR_sa });
		_camera->SetTarget({ 0.0f, (delimitNum.y / 2.0f) * delimitSize - UD_sa  ,(delimitNum.z / 2.0f) * delimitSize + LR_sa });
	}
	//左
	else if (_cameraNum == 6) {
		_camera->SetEye({ -delimitNum.x * delimitSize, (delimitNum.y / 2.0f) * delimitSize - UD_sa , (delimitNum.z / 2.0f) * delimitSize - LR_sa });
		_camera->SetTarget({ 0.0f, (delimitNum.y / 2.0f) * delimitSize - UD_sa  ,(delimitNum.z / 2.0f) * delimitSize - LR_sa });
	}
}

void Scene1::FrameReset()
{
	map->FrameReset();
}