#pragma once
#include "Object/3d/Object3d.h"
#include "Math/Vector3.h"

class Player
{
private:

	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMVECTOR = DirectX::XMVECTOR;

public:

	Player();
	~Player(){};

	void Update();

	void Draw();

private:

	void Move();
	void Collider();

public:

	Vector3 GetPos() { return pos; }

	void SetMoveRota(const float _rota) { moveRota = _rota; }

private:

	std::unique_ptr<Model> model = nullptr;
	std::unique_ptr<Object3d> object = nullptr;

	Vector3 pos;
	Vector3 moveVec;
	float moveRota;

	bool onGround = true;
	// 落下ベクトル
	DirectX::XMVECTOR fallV;

};