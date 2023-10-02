#pragma once

static const int groundModelNum = 2;
static const float groundSize = 15.0f;

//向いている方向
enum class GroundType {
	right,
	left,
	up,
	down,
	back,
	front,
	size,
};

enum class GroundState {
	stop,//可動なし
	change,//変更
	attack,//攻撃
	size,
};