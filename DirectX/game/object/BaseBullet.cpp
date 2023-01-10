#include "BaseBullet.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "GameHelper.h"

using namespace DirectX;

std::unique_ptr<Model> BaseBullet::model;
std::array<std::unique_ptr<InstanceObject>, BaseBullet::object_max_num> BaseBullet::bullet;
int BaseBullet::usingNum = 0;
std::array<std::unique_ptr<Emitter>, BaseBullet::effect_max_num>  BaseBullet::effect;
std::array<int, BaseBullet::effect_max_num> BaseBullet::effectNum;

void BaseBullet::StaticInitialize()
{
	//�e
	model = Model::CreateFromOBJ("NormalCube");

	for (auto& i : bullet)
	{
		i = InstanceObject::Create(model.get());
		i->SetBloom(true);
		i->SetLight(false);
	}

	ParticleManager::LoadTexture("effect1", "Resources/particle/effect1.png");//�ėp�G�t�F�N�g

	for (auto& i : effect) {
		i = Emitter::Create("effect1");
		i->SetBloom();
	}
}

void BaseBullet::Initialize()
{
	isAlive = true;
	scale = 1.0f;
	rotate = { 0,0,0 };
}

void BaseBullet::StaticUpdate() {
	for (int i = 0; i < effect_max_num; i++) {
		effectNum[i] = effect[i]->GetCreateNum();
	}
}

void BaseBullet::Update()
{
	if (!isAlive) { return; }

	//�ő�l�ɂ������琶���t���O������
	if (pos.x < 0.0f || pos.x>1000 || pos.y > 430 || pos.y < -1.0f || pos.z < 0.0f || pos.z>1000) {
		isAlive = false;
	}

	//�J�v�Z��
	Capsule capsule;
	capsule.startPosition = { pos.x,pos.y,pos.z };
	capsule.endPosition = { pos.x + move.x,pos.y + move.y,pos.z + move.z };
	capsule.radius = scale;

	if (CollisionManager::GetInstance()->QueryCapsule(capsule, COLLISION_ATTR_LANDSHAPE))
	{
		isAlive = false;
		return;
	}

	XMFLOAT3 bPos = pos;
	pos.x += move.x;
	pos.y += move.y;
	pos.z += move.z;

	for (auto& i : bullet)
	{
		if (!i->GetInstanceDrawCheck()) { continue; }
		i->DrawInstance(pos, { scale,scale,scale }, rotate, { color.x,color.y,color.z,1.0f });
		break;
	}

	//�ړ����Ă��鎞�Ɏc���Ƃ��ăp�[�e�B�N�����o��
	if(move.x==0&& move.y == 0 && move.z == 0){return;}

	//�ړ����Ă��鎞�Ɏc���Ƃ��ďo���p�[�e�B�N������
	for (int i = 0; i < effect_max_num; i++) {
		if (effectNum[i] >= 512) { continue; }
		effect[i]->InEmitter(10, pos, { 0,0,0 }, { 0,0,0 }, 5.0f, 0.0f,
			{ color.x,color.y,color.z,1.0f }, { color.x,color.y,color.z,1.0f });
		effectNum[i]++;
		break;
	}
}

void BaseBullet::Draw()
{
	for (auto& i : bullet) {
		if (i->GetInstanceDrawNum() == 0) { continue; }
		i->Draw();
	}
}

void BaseBullet::EffectDraw()
{
	for (int i = 0; i < effect_max_num; i++) {
		if (effectNum[i] == 0) { continue; }
		effect[i]->Update();
		effect[i]->Draw();
	}
}

void BaseBullet::Finalize()
{
	model.reset();
}