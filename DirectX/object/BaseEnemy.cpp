#include "BaseEnemy.h"

BaseEnemy::~BaseEnemy()
{
}

void BaseEnemy::Initialize()
{
	model[0] = Model::CreateFromOBJ("core");//�{�X�̃R�A
	model[1] = Model::CreateFromOBJ("Square");//�{�X�̑�
}