#pragma once
#include "Object3d.h"

class FrictionObject : public Object3d
{
public:
	/// <summary>
	/// 3D�I�u�W�F�N�g����
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<FrictionObject> Create(Model* model = nullptr);
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

	//�d��
	const float gravity = 9.8f / 60.0f;
	//���C�W��
	const float coefficientFriction = 0.8f;
	//����(kg)
	const float mass = 10.0f;
	//�����R��
	const float normalForce = mass * gravity;
	//�Î~���C
	const float restFriction = coefficientFriction * normalForce;
	//�ړ������ɂ������
	float movePower = 0;
};