#include "Camera.h"
#include "WindowApp.h"

using namespace DirectX;

Camera::Camera()
{
	aspectRatio = (float)WindowApp::GetWindowWidth() / WindowApp::GetWindowHeight();

	// 射影行列の計算
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		aspectRatio,
		0.1f, 1200.0f//奥行/手前,最奥
	);
}

std::unique_ptr<Camera> Camera::Create()
{
	// Spriteのインスタンスを生成
	Camera* instance = new Camera();
	if (instance == nullptr) {
		return nullptr;
	}

	//ユニークポインタを返す
	return std::unique_ptr<Camera>(instance);
}

void Camera::Update()
{
	XMFLOAT3 inoutEye = { ShakeDifference.x + eye.x,ShakeDifference.y + eye.y,ShakeDifference.z + eye.z };

	matView = XMMatrixLookAtLH(XMLoadFloat3(&inoutEye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void Camera::StartCameraShake(int strength)
{
	int x = (rand() % strength * 2) - strength;
	int y = (rand() % strength * 2) - strength;

	ShakeDifference = { (float)x ,(float)y,0.0f };
}

void Camera::EndCameraShake()
{
	ShakeDifference = { 0.0f,0.0f,0.0f };
}