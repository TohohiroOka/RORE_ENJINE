#include "EnemyABullet.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

using namespace DirectX;

std::unique_ptr<EnemyABullet> EnemyABullet::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	EnemyABullet* instance = new EnemyABullet();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize(_pos, _moveVec);

	return std::unique_ptr<EnemyABullet>(instance);
}

void EnemyABullet::Initialize(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec)
{
	object = Object3d::Create(model.get());

	// �R���C�_�[�̒ǉ�
	float radius = 3.0f;
	object->SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	object->GetCollider()->SetAttribute(COLLISION_ATTR_ALLIES);

	isAlive = true;
	pos = _pos;
	object->SetPosition(pos);
	moveVec = _moveVec;
	scale = 3;
	object->SetScale({ scale ,scale ,scale });

	object->Update();
}

void EnemyABullet::Update()
{
	const float speed = 1.0f;

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

	Ray ray;
	ray.start = { pos.x,pos.y,pos.z,1 };
	ray.dir = { moveVec.x,moveVec.y,moveVec.z,0 };
	RAYCAST_HIT raycastHit;
	//�n�`�ɓ������Ă����琶���t���O������
	if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, scale))
	{
		//XMFLOAT3 hitPos = { raycastHit.inter.m128_f32[0], raycastHit.inter.m128_f32[1], raycastHit.inter.m128_f32[2] };
		//float x = powf(hitPos.x - pos.x, 2);
		//float y = powf(hitPos.y - pos.y, 2);
		//float z = powf(hitPos.z - pos.z, 2);
		//float eneTobul1 = sqrt(x + y + z);

		//x = powf(pos.x + moveVec.x * speed, 2);
		//y = powf(pos.y + moveVec.y * speed, 2);
		//z = powf(pos.z + moveVec.z * speed, 2);
		//float eneTobul2 = sqrt(x + y + z);

		//if (eneTobul1 < eneTobul2)
		//{
			isAlive = false;
		//}
	}

	//Capsule cupsule;
	//cupsule.startPosition = { pos.x,pos.y,pos.z };
	//cupsule.endPosition = { pos.x - moveVec.x,pos.y - moveVec.y,pos.z - moveVec.z };
	//cupsule.radius = scale / 2.0f;

	//if (CollisionManager::GetInstance()->QueryCapsule(cupsule, COLLISION_ATTR_LANDSHAPE))
	//{
	//	//XMFLOAT3 hitPos = { raycastHit.inter.m128_f32[0], raycastHit.inter.m128_f32[1], raycastHit.inter.m128_f32[2] };
	//	//float x = powf(hitPos.x - pos.x, 2);
	//	//float y = powf(hitPos.y - pos.y, 2);
	//	//float z = powf(hitPos.z - pos.z, 2);
	//	//float eneTobul1 = sqrt(x + y + z);

	//	//x = powf(pos.x + moveVec.x * speed, 2);
	//	//y = powf(pos.y + moveVec.y * speed, 2);
	//	//z = powf(pos.z + moveVec.z * speed, 2);
	//	//float eneTobul2 = sqrt(x + y + z);

	//	//if (eneTobul1 < eneTobul2)
	//	//{
	//	isAlive = false;
	//	//}
	//}
	
	//�ő�l�ɂ������琶���t���O������
	if (pos.y > 500 || pos.z < 0.0f) {
		isAlive = false;
	}

	pos.x += moveVec.x * speed;
	pos.y += moveVec.y * speed;
	pos.z += moveVec.z * speed;

	object->SetPosition(pos);
	object->Update();
}

void EnemyABullet::Draw()
{
	if (!isAlive) { return; }
	object->Draw();
}
