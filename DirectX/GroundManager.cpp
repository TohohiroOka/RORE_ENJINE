#include "GroundManager.h"

GroundManager::GroundManager()
{
	for (int i = 0; i<int(GroundType::size); i++) {
		object[i].reset(new Ground(i));
	}

	//object[2].reset(new Ground(2));

}

void GroundManager::Initialize()
{
}

void GroundManager::Update()
{
	for (auto& i : object) {
		i->Update();
	}

	//object[2]->Update();

}

void GroundManager::Draw()
{
	for (auto& i : object) {
		i->Draw();
	}

	//object[2]->Draw();
}
