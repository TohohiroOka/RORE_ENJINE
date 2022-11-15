#pragma once
#include "InstanceObject.h"

class BaseBullet
{
protected: // �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;

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
	static const int object_max_num = 20;
	//�I�u�W�F�N�g
	static std::array<std::unique_ptr<InstanceObject>, object_max_num> object;
	//���݂̎g�p��
	static int usingNum;

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
	float scale;
	//�p�x
	XMFLOAT3 rotate;
	//�F
	XMFLOAT3 color = { 1,1,1 };

public:

	void SetIsAlive(bool _isAlive) { isAlive = _isAlive; }
	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
	XMFLOAT3 GetMoveVec() { return moveVec; }
	XMFLOAT3 GetMove() { return move; }
};

