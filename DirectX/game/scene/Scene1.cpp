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

// �X�N���[�����W�����[���h���W�ɕϊ�
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
	// �t�s��
   // �t�ϊ�
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

	//�t���[���̍ŏ��ɏ������������
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
	//�}�E�X���W
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
		//�I�u�W�F�N�g�ݒu
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
	//��
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

	//�}�b�v�T�C�Y
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

	//�ݒu�t���O�Ǘ�
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

	//�T�C�Y�ύX
	if (mapChangeSize.x != mapSize.x || mapChangeSize.y != mapSize.y || mapChangeSize.z != mapSize.z) {
		mapChange = true;
	}

	input = nullptr;
}

void Scene1::CameraUpdate(Camera* _camera)
{
	DirectInput* input = DirectInput::GetInstance();
	//���E�ړ�
	if (!kaburi) {
		const float Tgspeed = 1.0f;
		if (input->PushKey(DIK_RIGHT)) { cameraTarget.x -= Tgspeed; }//�E����
		if (input->PushKey(DIK_LEFT)) { cameraTarget.x += Tgspeed; }//������
		if (input->PushKey(DIK_DOWN)) { cameraTarget.y -= 0.01f + abs(cameraTarget.y) * 0.01f; }//������
		if (input->PushKey(DIK_UP)) { cameraTarget.y += 0.01f + abs(cameraTarget.y) * 0.01f; }//�����

		//�㉺�����̊p�x����
		if (cameraTarget.y <= -13) { cameraTarget.y = -13; }//������
		if (cameraTarget.y >= 13) { cameraTarget.y = 13; }//�㐧��

		//���̐���
		if (cameraTarget.x > 360) { cameraTarget.x = 0; }
		if (cameraTarget.x < -360) { cameraTarget.x = 0; }
	}

	//�J�����ړ�
	float Pspeed = 1.0f;
	XMFLOAT2 radian = { XMConvertToRadians(cameraTarget.x + 90),XMConvertToRadians(cameraTarget.x) };

	if (!kaburi) {
	//�E����
		if (input->PushKey(DIK_S)) {
			cameraPos.x += Pspeed * cos(radian.x);
			cameraPos.z += Pspeed * sin(radian.x);
		}
		//������
		if (input->PushKey(DIK_W)) {
			cameraPos.x -= Pspeed * cos(radian.x);
			cameraPos.z -= Pspeed * sin(radian.x);
		}
		//������
		if (input->PushKey(DIK_D)) {
			cameraPos.x -= Pspeed * cos(radian.y);
			cameraPos.z -= Pspeed * sin(radian.y);
		}
		//�����
		if (input->PushKey(DIK_A)) {
			cameraPos.x += Pspeed * cos(radian.y);
			cameraPos.z += Pspeed * sin(radian.y);
		}
		//������
		if (input->PushKey(DIK_SPACE)) {
			cameraPos.y += Pspeed;
		}
		//�����
		if (input->PushKey(DIK_LSHIFT)) {
			cameraPos.y -= Pspeed;
		}
	}

	_camera->SetEye(cameraPos);
	_camera->SetTarget({ cameraPos.x - cosf(radian.x),cameraPos.y + cameraTarget.y ,cameraPos.z - sinf(radian.x) });

	camera = _camera;
}