#include "LatticeBeam.h"
#include "GameHelper.h"

using namespace DirectX;

LatticeBeam::LatticeBeam(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, float _speed, const XMFLOAT3& _color)
{
	timer = 0;
	pos = _pos;
	moveVec = _moveVec;
	const float speed = 15.0f;
	move = { moveVec.x * speed , moveVec.y * speed , moveVec.z * speed };
	color = { _color.x,_color.y,_color.z,1.0f };
	rotate = { 0,0,0 };	

	if (moveVec.x != 0) {
		rotate.z = 90;
	}else if (moveVec.y != 0) {
		rotate.y = 90;
	}
}

std::unique_ptr<LatticeBeam> LatticeBeam::Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, float _speed, const XMFLOAT3& _color)
{
	// 3Dオブジェクトのインスタンスを生成
	LatticeBeam* instance = new LatticeBeam(_pos, _moveVec, _speed, _color);
	if (instance == nullptr) {
		return nullptr;
	}

	instance->Initialize();

	return std::unique_ptr<LatticeBeam>(instance);
}

void LatticeBeam::Update()
{
	timer++;
	const int maxTimer = 200;

	//消去するためにalphaを引く
	if (timer > maxTimer) {
		color.w -= 0.01f;
		//alpha0で消去
		if (color.w<0.05f){
			isAlive = false;
		}
	}

	//移動
	if (move.x != 0 || move.y != 0 || move.z != 0) {
		Move();
	}

	BaseBeam::Update();
}

void LatticeBeam::Move()
{
	//符号/false->- true->+
	bool sign = false;

	if (move.x != 0){
		sign = move.x > 0;
		if (!(!sign && pos.x < mapSize / 2.0f)&&!(sign && pos.x > mapSize / 2.0f)) {
			return;
		}
		move.x = 0.0f;
	}
	else if (move.y != 0) {
		sign = move.y > 0;
		if (!(!sign && pos.y < mapSize / 2.0f) && !(sign && pos.y > mapSize / 2.0f)) {
			return;
		}
		move.y = 0.0f;
	}
	else if (move.z != 0) {
		sign = move.z > 0;
		if (!(!sign && pos.z < mapSize / 2.0f) && !(sign && pos.z > mapSize / 2.0f)) {
			return;
		}
		move.z = 0.0f;
	}

	return;
}
