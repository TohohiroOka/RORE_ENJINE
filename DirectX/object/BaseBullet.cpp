#include "BaseBullet.h"

std::unique_ptr<Model> BaseBullet::model;

void BaseBullet::StaticInitialize()
{
	model = Model::CreateFromOBJ("Square");//�v���C���[�̒e
}

void BaseBullet::Finalize()
{
	model.reset();
}
