#pragma once

static const int groundModelNum = 2;
static const float groundSize = 15.0f;

//�����Ă������
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
	stop,//���Ȃ�
	change,//�ύX
	attack,//�U��
	size,
};