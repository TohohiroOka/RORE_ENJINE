#pragma once
#include "BaseBeam.h"

class LatticeBeam : public BaseBeam
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <param name="_speed">���x</param>
	/// <param name="_color">�F</param>
	LatticeBeam(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec, float _speed, const XMFLOAT3& _color);
	~LatticeBeam() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <param name="_speed">���x</param>
	/// <param name="_color">�F</param>
	/// <returns></returns>
	static std::unique_ptr<LatticeBeam> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec,
		float _speed, const XMFLOAT3& _color);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �ړ�
	/// </summary>
	void Move();

private:

	int timer;
};