#include "BeamManager.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "GameHelper.h"

std::list<std::unique_ptr<BaseBeam>> BeamManager::beam;

BeamManager::~BeamManager()
{
	std::list<std::unique_ptr<BaseBeam>>().swap(beam);

	BaseBeam::Finalize();
}

std::unique_ptr<BeamManager> BeamManager::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	BeamManager* instance = new BeamManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->Initialize();

	return std::unique_ptr<BeamManager>(instance);
}

void BeamManager::Initialize()
{
	BaseBeam::StaticInitialize();
}

void BeamManager::Update()
{
	for (auto& i : beam)
	{
		if (!i->GetIsAlive()) { continue; }
		i->Update();
	}

	//��~�����e�𐏎�����
	{
		for (auto it = beam.begin(); it != beam.end();)
		{
			if (!it->get()->GetIsAlive())
			{
				// �폜���ꂽ�v�f�̎����w���C�e���[�^���Ԃ����B
				it = beam.erase(it);
			}
			// �v�f�폜�����Ȃ��ꍇ�ɁA�C�e���[�^��i�߂�
			else {
				++it;
			}
		}
	}
}

void BeamManager::Draw()
{
	BaseBeam::Draw();
}

void BeamManager::Reset()
{
	beam.clear();
}

bool BeamManager::CheckPlayerBulletToEnemyCollision(const XMFLOAT3& _pos, float _scale)
{
	return false;
}