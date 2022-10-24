#include "BossBulletF.h"
#include "BulletManager.h"
#include "GameHelper.h"

using namespace DirectX;

BossBulletF::BossBulletF(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	pos = _pos;
	moveVec = _moveVec;
	timer = 0;
	maxTimer = int(Randomfloat(40)) + 20;
}

std::unique_ptr<BossBulletF> BossBulletF::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, const XMFLOAT3& _color)
{
	// 3Dオブジェクトのインスタンスを生成
	BossBulletF* instance = new BossBulletF(_pos, _moveVec);
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();
	instance->object->SetColor({ _color.x,_color.y,_color.z,1.0f });

	return std::unique_ptr<BossBulletF>(instance);
}

void BossBulletF::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();

	const float speed = 15.0f;
	move = { moveVec.x * speed , moveVec.y * speed , moveVec.z * speed };
}

void BossBulletF::Update()
{
	timer++;

	BaseBullet::Update();

	if (timer > maxTimer)
	{
		//弾ける弾の数
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