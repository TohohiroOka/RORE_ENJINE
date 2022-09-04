#include "BaseEnemy.h"

BaseEnemy::~BaseEnemy()
{
}

void BaseEnemy::Initialize()
{
	model[0] = Model::CreateFromOBJ("core");//ボスのコア
	model[1] = Model::CreateFromOBJ("Square");//ボスの体
}