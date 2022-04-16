#pragma once
#include "Object3d.h"

class FreeFallCircle : public Object3d
{
public:
	/// <summary>
	/// 3D�I�u�W�F�N�g����
	/// </summary>
	/// <returns></returns>
	static FreeFallCircle* Create(Model* model = nullptr);

public:

	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	bool Initialize() override;

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update() override;

private:
	bool onGround = true;
	float addG = 0;
};