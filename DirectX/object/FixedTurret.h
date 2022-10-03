#pragma once
#include "Object3d.h"

class FixedTurret
{
private:

	using XMFLOAT3 = DirectX::XMFLOAT3;

public:

	FixedTurret() {};
	~FixedTurret() {};

	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	/// <returns></returns>
	static std::unique_ptr<FixedTurret> Create(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	void Initialize(const XMFLOAT3& _pos, const XMFLOAT3& _moveVec);

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Draw();

	/// <summary>
	/// ������
	/// </summary>
	void Reset();
	
private:

	static std::unique_ptr<Model> model;
	std::unique_ptr<Object3d> object;
	//���W
	XMFLOAT3 pos;
	//���˕���
	bool isAddSub;
	XMFLOAT3 moveVec;

	//���ˎ���
	int timer;
};

