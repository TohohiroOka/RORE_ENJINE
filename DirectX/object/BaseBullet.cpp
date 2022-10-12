#include "BaseBullet.h"

std::unique_ptr<Model> BaseBullet::model;

void BaseBullet::StaticInitialize()
{
	model = Model::CreateFromOBJ("Square");//ƒvƒŒƒCƒ„[‚Ì’e
}

void BaseBullet::Finalize()
{
	model.reset();
}
