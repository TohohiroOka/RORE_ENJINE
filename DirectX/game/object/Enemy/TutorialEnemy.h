#pragma once
#include "BaseEnemy.h"

class TutorialEnemy : public BaseEnemy
{
public:

	TutorialEnemy(const XMFLOAT3& _pos);
	~TutorialEnemy() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <returns></returns>
	static std::unique_ptr<TutorialEnemy> Create(const XMFLOAT3& _pos);
};