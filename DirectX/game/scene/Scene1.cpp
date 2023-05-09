#include "Scene1.h"
#include "SceneManager.h"
#include "DirectInput.h"
#include "XInputManager.h"
#include "DebugText.h"
#include "WindowApp.h"
#include <imgui.h>

using namespace DirectX;

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

	map->Update(pout, target, kaburi);

	//DebugText* text = DebugText::GetInstance();

	//text->Print("move          : WASD", 20, 25);
	//text->Print("camera        : ARROW", 20, 40);
	//text->Print("Add or Delete : MOUSE LEFT", 20, 55);
	//text->Print("Add or Delete model change : M", 20, 70);
	//text->Print("Line Draw Change: P", 20, 85);
	//text->Print("Output        : O", 20, 100);

	//text = nullptr;
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
	//�t�@�C����
	static char fileName[36] = "";
	//�}�b�v�T�C�Y
	const XMINT3 mapSize = map->GetDelimitNum();
	mapChangeSize = mapSize;

	std::array<bool, 3> isSetFlag = isSetObject;

	ImGui::Begin("MapEditor");
	ImGui::SetWindowPos(ImVec2(imguiPos.x, imguiPos.y));
	ImGui::SetWindowSize(ImVec2(imguiMax.x, imguiMax.y));

	ImGui::InputText("FileName", fileName, sizeof(fileName));
	//�o��
	if (ImGui::Button("import")) {
		map->OutputMap(fileName, 30);
	}
	ImGui::SliderInt("MapSize : X", &mapChangeSize.x, 1, 20);
	ImGui::SliderInt("MapSize : Y", &mapChangeSize.y, 1, 20);
	ImGui::SliderInt("MapSize : Z", &mapChangeSize.z, 1, 20);
	ImGui::Checkbox("Player", &isSetObject[0]);
	ImGui::Checkbox("Goal", &isSetObject[1]);
	ImGui::Checkbox("NormalObject", &isSetObject[2]);
	ImGui::Checkbox("Line Draw", &isDrawLine);
	ImGui::Checkbox("Box Delete", &isDelete);

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
}

void Scene1::CameraUpdate(Camera* _camera)
{
	DirectInput* input = DirectInput::GetInstance();
	//���E�ړ�
	if (!kaburi) {
		const float Tgspeed = 1.0f;
		if (input->PushKey(DIK_RIGHT)) { cameraTarget.x -= Tgspeed; }//�E����
		if (input->PushKey(DIK_LEFT)) { cameraTarget.x += Tgspeed; }//������
		if (input->PushKey(DIK_UP)) { cameraTarget.y -= Tgspeed; }//�����
		if (input->PushKey(DIK_DOWN)) { cameraTarget.y += Tgspeed; }//������

		//�㉺�����̊p�x����
		if (cameraTarget.y <= -48) { cameraTarget.y = -48; }//������
		if (cameraTarget.y >= 60) { cameraTarget.y = 60; }//�㐧��

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
	_camera->SetTarget({ cameraPos.x - cosf(radian.x),cameraPos.y - sinf(XMConvertToRadians(cameraTarget.y)) ,cameraPos.z - sinf(radian.x) });

	camera = _camera;
}