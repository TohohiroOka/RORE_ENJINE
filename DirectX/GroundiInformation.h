#pragma once

static const int groundModelNum = 2;
static const float groundSize = 15.0f;

//Œü‚¢‚Ä‚¢‚é•ûŒü
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
	stop,//‰Â“®‚È‚µ
	change,//•ÏX
	attack,//UŒ‚
	size,
};