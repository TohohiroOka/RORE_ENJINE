#pragma once
#include "Object3d.h"

class BossBeam
{
protected: // �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;

public:

	BossBeam() {};
	~BossBeam() {};

	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// ����
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<BossBeam> Create();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_color">�F</param>
	void Update(const XMFLOAT3& _pos, const XMFLOAT3& _color);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

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
	//�r�[���̕���
	XMFLOAT3 moveVec;
	//�傫��
	float scale;
	//�p�x(xy,xz)
	XMFLOAT2 angle;

public:

	void SetIsAlive(bool _isAlive) { isAlive = _isAlive; }
	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
	XMFLOAT3 GetMoveVec() { return moveVec; }

};

