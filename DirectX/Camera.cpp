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

void Camera::Update()
{
	XMFLOAT3 inoutEye = { ShakeDifference.x + eye.x,ShakeDifference.y + eye.y,ShakeDifference.z + eye.z };

	matView = XMMatrixLookAtLH(XMLoadFloat3(&inoutEye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void Camera::FpsCamera(XMFLOAT3& position, XMFLOAT3& move, float speed)
{
	const float radian = XM_PI / 180.0f;

	XMMATRIX rotM = XMMatrixIdentity();
	XMVECTOR v0 = { 0, 0, -200, 0 };
	XMVECTOR v = XMVector3TransformNormal(v0, rotM);

	target = { position.x - cosf(static_cast<double>(move.x * radian)) * speed,
		position.y - sinf(static_cast<double>(move.y * radian)) * speed,
		position.z - sinf(static_cast<double>(move.z * radian)) * speed };
	XMVECTOR targetV = { target.x, target.y, target.z, 0 };

	XMVECTOR eyeV = targetV + v;

	eye = { eyeV.m128_f32[0], eyeV.m128_f32[1], eyeV.m128_f32[2] };

	eye.x = position.x;
	eye.y = position.y;
	eye.z = position.z;
}

void Camera::TpsCamera(XMFLOAT3 distance)
{
	target = { position.x,position.y,position.z };

	eye.x = position.x + distance.x;
	eye.y = position.y + distance.y;
	eye.z = position.z + distance.z;
}

void Camera::CameraShake(int strength)
{
	int x = (rand() % strength * 2) - strength;
	int y = (rand() % strength * 2) - strength;

	ShakeDifference = { (float)x ,(float)y,0.0f };
}