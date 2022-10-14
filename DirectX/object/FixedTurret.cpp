#include "FixedTurret.h"
#include "BulletManager.h"

std::unique_ptr<Model> FixedTurret::model;

void FixedTurret::StaticInitialize()
{
	model = Model::CreateFromOBJ("Square1");
}

std::unique_ptr<FixedTurret> FixedTurret::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	// 3Dオブジェクトのインスタンスを生成
	FixedTurret* instance = new FixedTurret();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize(_pos, _moveVec);

	return std::unique_ptr<FixedTurret>(instance);
}

void FixedTurret::Initialize(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	object = Object3d::Create(model.get());

	//座標
	pos = _pos;
	object->SetPosition(pos);
	object->SetScale({ 5,5,5 });
	//発射方向
	moveVec = _moveVec;
	object->Update();

	isAddSub = rand() % 2;

	timer = 0;
}

void FixedTurret::Update()
{
	timer++;

	if (timer % 50 == 9)
	{
		if (isAddSub)
		{
			moveVec.y += 0.05f;
			if (moveVec.y > 0.4f)
			{
				isAddSub = false;
			}
		}
		else
		{
			moveVec.y -= 0.05f;
			if (moveVec.y < 0.05f)
			{
				isAddSub = true;
			}
		}
		BulletManager::SetFixedTurretBullet(pos, moveVec);
	}

	object->Update();
}

void FixedTurret::Draw()
{
	object->Draw();
}

void FixedTurret::Reset()
{
}
