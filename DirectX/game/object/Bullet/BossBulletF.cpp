#include "BossBulletF.h"
#include "BulletManager.h"
#include "GameHelper.h"

using namespace DirectX;

BossBulletF::BossBulletF(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color)
{
	pos = _pos;
	moveVec = _moveVec;
	const float speed = 15.0f;
	move = { moveVec.x * speed , moveVec.y * speed , moveVec.z * speed };
	timer = 0;
	maxTimer = int(Randomfloat(40)) + 20;
	color = _color;
}

std::unique_ptr<BossBulletF> BossBulletF::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	BossBulletF* instance = new BossBulletF(_pos, _moveVec, _color);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<BossBulletF>(instance);
}

void BossBulletF::Update()
{
	timer++;

	BaseBullet::Update();

	if (timer > maxTimer)
	{
		//�e����e�̐�
		const int bulletNum = 25;
		float speed = 20.0f;
		for (int i = 0; i < bulletNum; i++)
		{
			BulletManager::SetBossBulletFF(pos, speed,
				{ float(i % 10) / 10.0f, float(i / bulletNum), float(i) / 10.0f });
		}
		isAlive = false;
	}
}