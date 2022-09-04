#include "FirstBoss.h"

using namespace DirectX;

//�R�A����̋���
const XMFLOAT3 FirstBoss::bodyDistance[bossBodyNum] = {
	{-10,10,-10}, //�O����
	{-10,-10,-10},//�O����
	{10,10,-10},  //�O�E��
	{10,-10,-10}, //�O�E��
	{-10,10,10},  //�㍶��
	{-10,-10,10}, //�㍶��
	{10,10,10},   //��E��
	{10,-10,10},  //��E��
};

FirstBoss::~FirstBoss()
{
}

std::unique_ptr<FirstBoss> FirstBoss::Create()
{
	//�C���X�^���X�𐶐�
	FirstBoss* instance = new FirstBoss();
	if (instance == nullptr) {
		return nullptr;
	}

	//������
	instance->Initialize();

	return std::unique_ptr<FirstBoss>(instance);
}

void FirstBoss::Initialize()
{
	BaseEnemy::Initialize();

	core = Object3d::Create(model[0].get());
	core->SetPosition({ 500,100,500 });
	core->SetScale({ 5,5,5 });
	for (int i = 0; i < bossBodyNum; i++)
	{
		body[i] = Object3d::Create(model[1].get());
		XMFLOAT3 pos = core->GetPosition();
		pos.x += bodyDistance[i].x;
		pos.y += bodyDistance[i].y;
		pos.z += bodyDistance[i].z;
		body[i]->SetPosition(pos);
		body[i]->SetScale({ 5,5,5 });
	}
}

void FirstBoss::Update()
{
	core->Update();
	for (int i = 0; i < bossBodyNum; i++)
	{
		body[i]->Update();
	}
}

void FirstBoss::Draw()
{
	core->Draw();
	for (int i = 0; i < bossBodyNum; i++)
	{
		body[i]->Draw();
	}
}

XMFLOAT3 FirstBoss::GetPosition()
{
	XMFLOAT3 pos = core->GetPosition();
	return pos;
}
