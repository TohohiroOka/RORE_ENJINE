#include "EnemyA.h"
#include "BulletManager.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

#include "DirectInput.h"

using namespace DirectX;

std::unique_ptr<EnemyA> EnemyA::Create(const XMFLOAT3& _pos)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	EnemyA* instance = new EnemyA();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize(_pos);

	return std::unique_ptr<EnemyA>(instance);
}

void EnemyA::Initialize(const XMFLOAT3& _pos)
{
	timer = 0;

	object = Object3d::Create(model.get());

	// �R���C�_�[�̒ǉ�
	float radius = 3.0f;
	object->SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	object->GetCollider()->SetAttribute(COLLISION_ATTR_ENEMYS);

	isAlive = true;
	pos = _pos;
	object->SetPosition(pos);
	moveVec = { 0,0,0 };
	scale = 3;
	object->SetScale({ scale ,scale ,scale });
	object->Update();

	onGround = false;
}

void EnemyA::Update()
{
	timer++;

	//if (timer % 50 == 1)
	//{
	//	float angle = 360.0f / float(bulletNum);
	//	for (int i = 0; i < bulletNum; i++)
	//	{
	//		float nowAngle = angle * i;
	//		float radiun = XMConvertToRadians(nowAngle);
	//		BulletManager::SetEnemyABullet(pos, { cos(radiun),0.0f,sin(radiun) });
	//	}
	//}

	if (DirectInput::GetInstance()->TriggerKey(DIK_P))
	{
		BulletManager::SetEnemyABullet(pos, { 1,0,0 });
	}

	const float speed = 1.0f;
	pos.x += moveVec.x * speed;
	pos.y += moveVec.y * speed;
	pos.z += moveVec.z * speed;

	object->SetPosition(pos);
	object->Update();

	MapCollider();
	object->SetPosition(pos);
	object->Update();
}

void EnemyA::Draw()
{
	if (!isAlive) { return; }
	object->Draw();
}

void EnemyA::SetMove(const XMFLOAT3& _pos)
{
	//���K�������e�ۂ̈ړ��x�N�g���Z�o
	XMFLOAT3 vecPt = {};
	vecPt.x = _pos.x - pos.x;
	vecPt.y = _pos.y - pos.y;
	vecPt.z = _pos.z - pos.z;

	//���K��
	float x = powf(vecPt.x, 2);
	float y = powf(vecPt.y, 2);
	float z = powf(vecPt.z, 2);
	float dist = sqrt(x + y + z);
	dist = 1.0f / dist;
	moveVec.x = vecPt.x * dist;
	moveVec.y = vecPt.y * dist;
	moveVec.z = vecPt.z * dist;
}

void EnemyA::MapCollider()
{
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(object->GetCollider());
	assert(sphereCollider);

	// �N�G���[�R�[���o�b�N�N���X
	class PlayerQueryCallback : public QueryCallback
	{
	public:
		PlayerQueryCallback(Sphere* sphere) : sphere(sphere) {};

		// �Փˎ��R�[���o�b�N�֐�
		bool OnQueryHit(const QUERY_HIT& info) {

			const XMVECTOR up = { 0,1,0,0 };

			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			// �n�ʔ��肵�����l
			const float threshold = cosf(XMConvertToRadians(90.0f));

			if (-threshold < cos && cos < threshold) {
				sphere->center += info.reject;
				move += info.reject;
			}

			return true;
		}

		Sphere* sphere = nullptr;
		DirectX::XMVECTOR move = {};
	};
	PlayerQueryCallback callback(sphereCollider);

	// ���ƒn�`�̌�����S����
	CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_LANDSHAPE);
	// �����ɂ��r�˕�������
	pos.x += callback.move.m128_f32[0];
	pos.y += callback.move.m128_f32[1];
	pos.z += callback.move.m128_f32[2];

	// ���̏�[���狅�̉��[�܂ł̃��C�L���X�g
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
	RAYCAST_HIT raycastHit;

	// �X���[�Y�ɍ������ׂ̋z������
	const float adsDistance = 1.0f;

	//���̔���
	{
		// �ڒn���ێ�
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance)) {
			float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			pos.y -= a;
		}
	}
	//���̔���
	{
		ray.dir = { -1.0f,0.0f,0.0f,0.0f };
		// �ڒn���ێ�
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
		{
			float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			pos.x -= a;
		}
	}
	//�E�̔���
	{
		ray.dir = { 1.0f,0.0f,0.0f,0.0f };
		// �ڒn���ێ�
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
		{
			float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			pos.x += a;
		}
	}
	//�O�̔���
	{
		ray.dir = { 0.0f,0.0f,-1.0f,0.0f };
		// �ڒn���ێ�
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
		{
			float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			pos.z -= a;
		}
	}
	//��̔���
	{
		ray.dir = { 0.0f,0.0f,1.0f,0.0f };
		// �ڒn���ێ�
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
		{
			float a = (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			pos.z += a;
		}
	}
}