#include "BaseEnemy.h"

std::unique_ptr<Model> BaseEnemy::model;

void BaseEnemy::StaticInitialize()
{
	model = Model::CreateFromOBJ("Square2");//ƒvƒŒƒCƒ„[‚Ì’e
}

void BaseEnemy::Finalize()
{
	model.reset();
}