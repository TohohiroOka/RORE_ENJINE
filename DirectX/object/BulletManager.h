#pragma once
#include "BaseBullet.h"

class BulletManager
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;

public:

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Draw();

private:

	std::unique_ptr<BaseBullet> bullet = nullptr;

};

