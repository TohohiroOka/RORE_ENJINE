#pragma once
#include "Object3d.h"

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
	virtual void Draw();

	/// <summary>
	/// delete����
	/// </summary>
	static void Finalize();

protected:

	//���f��
	static std::unique_ptr<Model> model;

protected:

	//�I�u�W�F�N�g
	std::unique_ptr<Object3d> object;
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

public:

	void SetIsAlive(bool _isAlive) { isAlive = _isAlive; }
	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
	XMFLOAT3 GetMoveVec() { return moveVec; }
	XMFLOAT3 GetMove() { return move; }
};

