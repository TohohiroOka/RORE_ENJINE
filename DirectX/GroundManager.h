#pragma once
#include "Ground.h"

class GroundManager
{
public:
	GroundManager();
	~GroundManager() {};

	void Initialize();

	void Update();

	void Draw();

public:

	std::array<std::unique_ptr<Ground>, int(GroundType::size)>object;

};

