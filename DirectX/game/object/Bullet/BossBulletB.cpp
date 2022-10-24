#include "BossBulletB.h"
#include "BulletManager.h"

using namespace DirectX;

BossBulletB::BossBulletB(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	timer = 0;
	pos = _pos;
	moveVec = _moveVec;
}

std::unique_ptr<BossBulletB> BossBulletB::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color)
{
	// 3Dオブジェクトのインスタンスを生成
	BossBulletB* instance = new BossBulletB(_pos, _moveVec);
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();
	instance->object->SetColor({ _color.x,_color.y,_color.z,1.0f });

	return std::unique_ptr<BossBulletB>(instance);
}

void BossBulletB::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();

	const float speed = 8.0f;
	move = { moveVec.x * speed , moveVec.y * speed , moveVec.z * speed };
}

void BossBulletB::Update()
{
	timer++;

	BaseBullet::Update();

	if (timer > 40)
	{
		//弾ける弾の数
		const int bulletNum = 100;
		float addAngle = 360.0f / (bulletNum / 10);
		for (int i = 0; i < bulletNum; i++)
		{
			float nowAngleXY = addAngle * float(i % 10);
			float nowAngleXZ = addAngle * float(i / 10);
			float radiunXY = XMConvertToRadians(nowAngleXY);
			float radiunXZ = XMConvertToRadians(nowAngleXZ);

			BulletManager::SetBossBulletNormal(pos,
				{ cos(radiunXY) * cos(radiunXZ),cos(radiunXY) * sin(radiunXZ),sin(radiunXY) }, 8.0f,
				{ float(i % 10) / 10.0f, float(i / bulletNum), float(i) / 10.0f });
		}
		isAlive = false;
	}
}