#pragma once
#include "InstanceObject.h"

class BaseBeam
{
protected: // �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;

	struct OBEJCT {
		std::unique_ptr<InstanceObject> in;
		std::unique_ptr<InstanceObject> out;
	};

public:

	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	static void Draw();

	/// <summary>
	/// delete����
	/// </summary>
	static void Finalize();

protected:

	//���f��
	static std::unique_ptr<Model> model;
	//�I�u�W�F�N�g�̍ő�l
	static const int object_max_num = 2;
	//�I�u�W�F�N�g
	static std::array<OBEJCT, object_max_num> object;

protected:

	//�����t���O
	bool isAlive;
	//���W
	XMFLOAT3 pos;
	//�ړ��x�N�g��
	XMFLOAT3 moveVec;
	//�ړ����x
	XMFLOAT3 move;
	//�傫��
	XMFLOAT3 inScale;
	XMFLOAT3 outScale;
	//�p�x
	XMFLOAT3 rotate;
	//�F
	XMFLOAT4 color = { 1,1,1,1 };

public:

	void SetIsAlive(bool _isAlive) { isAlive = _isAlive; }
	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	XMFLOAT3 GetScale() { return outScale; }
	XMFLOAT3 GetMoveVec() { return moveVec; }
	XMFLOAT3 GetMove() { return move; }
};

