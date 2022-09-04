#pragma once
#include "Object3d.h"

class BaseBullet
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
	BaseBullet() = default;

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~BaseBullet() {};

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

protected:

	std::unique_ptr<Model> model = nullptr;
};

