#pragma once
#include "Object3d.h"
#include <array>

class BaseEnemy
{
protected: // �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	BaseEnemy() = default;

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~BaseEnemy();

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// ���W�擾
	/// </summary>
	/// <returns>���W</returns>
	virtual XMFLOAT3 GetPosition() = 0;

	///// <summary>
	///// ���S
	///// </summary>
	//void Dead() = 0;

protected://�萔

	static const int modelNum = 2;

protected:

	std::array<std::unique_ptr<Model>, modelNum> model;
};