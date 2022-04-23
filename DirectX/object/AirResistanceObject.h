#pragma once
#include "Object3d.h"

class AirResistanceObject : public Object3d
{

public:
	/// <summary>
	/// 3D�I�u�W�F�N�g����
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<AirResistanceObject> Create(Model* model = nullptr);
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

	//�ړ����Ă��邩
	bool onGround = true;
	//�d��
	const float gravity = 9.8f / 60.0f;
	//����(kg)
	const float mass = 5.0f;
	//���萔
	const float k = 0.9f;
	//���v�̏d��
	float addG = 0.0f;
	//���x
	float speed = 0.0f;
	//�����x
	float accel = 0.0f;
};