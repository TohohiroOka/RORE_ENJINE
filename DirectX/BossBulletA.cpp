#include "BossBulletA.h"

using namespace DirectX;

BossBulletA::BossBulletA(const XMFLOAT3& _pos, float _angleXZ, float _angleXY, const XMFLOAT4& _color)
{
	pos = _pos;
	angleXZ = _angleXZ;
	angleXY = _angleXY;
	distance = 0;
}

std::unique_ptr<BossBulletA> BossBulletA::Create(const XMFLOAT3& _pos, float _angleXZ, float _angleXY, const XMFLOAT4& _color)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	BossBulletA* instance = new BossBulletA(_pos, _angleXZ, _angleXY, _color);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<BossBulletA>(instance);
}

void BossBulletA::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();
}

void BossBulletA::Update()
{
	distance += 0.5f;

	//�p�x�X�V
	angleXZ += 0.5f;
	angleXY += 0.5f;

	//radian�ɕϊ�
	float radianXZ = XMConvertToRadians(angleXZ);
	float radianXY = XMConvertToRadians(angleXY);

	pos.x = distance * cosf(radianXZ) * cosf(radianXY);
	pos.y = distance * cosf(radianXZ) * sinf(radianXY);
	pos.z = distance * sinf(radianXZ);

	BaseBullet::MapCollider();
}
