#include "InterfaceObject3d.h"
#include "BaseCollider.h"
#include "CollisionManager.h"
#include "Camera.h"
#include "LightGroup.h"
#include "Model.h"
#include "Texture.h"

ID3D12Device* InterfaceObject3d::device = nullptr;
ID3D12GraphicsCommandList* InterfaceObject3d::cmdList = nullptr;
Camera* InterfaceObject3d::camera = nullptr;
LightGroup* InterfaceObject3d::light = nullptr;
Texture* InterfaceObject3d::cubetex;

using namespace DirectX;
using namespace Microsoft::WRL;

void InterfaceObject3d::StaticInitialize(ID3D12Device* _device)
{
	// 初期化チェック
	assert(!InterfaceObject3d::device);

	// nullptrチェック
	assert(_device);

	InterfaceObject3d::device = _device;

	// モデルの静的初期化
	Model::StaticInitialize(_device);
}

InterfaceObject3d::~InterfaceObject3d()
{
	//コライダー解放
	if (collider) {
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}

	constBuffB0.Reset();
}

void InterfaceObject3d::Update()
{
	UpdateWorldMatrix();

	// 当たり判定更新
	if (collider) {
		collider->Update();
	}
}

void InterfaceObject3d::Draw()
{
	Update();

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// ライトの描画
	light->Draw(cmdList, 2);
}

void InterfaceObject3d::ColliderDraw()
{
	collider->Draw();
}

void InterfaceObject3d::UpdateWorldMatrix()
{
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映

	// 親オブジェクトがあれば
	if (parent != nullptr) {
		matWorld *= parent->matWorld;
	}
}

void InterfaceObject3d::SetCollider(BaseCollider* _collider)
{
	_collider->SetObject(this);
	this->collider = _collider;
	// コリジョンマネージャに追加
	CollisionManager::GetInstance()->AddCollider(_collider);

	UpdateWorldMatrix();
	_collider->Update();
}

XMFLOAT3 InterfaceObject3d::GetWorldPosition()
{
	XMFLOAT3 worldpos;

	worldpos.x = matWorld.r[3].m128_f32[0];
	worldpos.y = matWorld.r[3].m128_f32[1];
	worldpos.z = matWorld.r[3].m128_f32[2];

	return worldpos;
}