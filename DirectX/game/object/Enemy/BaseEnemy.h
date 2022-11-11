#pragma once
#include "Object3d.h"

class BaseEnemy
{
protected: // �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;

	struct MOVE_LIST {
		//�ړ�����W
		XMFLOAT3 pos;
		//���̈ړ�����
		std::vector<int> destination;
	};

public:

	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticUpdate(const XMFLOAT3& _playerPos);

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
	/// �G�̈ړ������Z�b�g
	/// </summary>
	/// <param name="_pos">�v���C���[���W</param>
	void SetMove(const XMFLOAT3& _pos);

	/// <summary>
	/// �����߂��L�̓����蔻��
	/// </summary>
	void MapCollider();

	/// <summary>
	/// delete����
	/// </summary>
	static void Finalize();

protected:

	//�{�X���f��
	static std::array<std::unique_ptr<Model>, 2> bossModel;
	//enemyA���f��
	static std::unique_ptr<Model> enemyAModel;
	//�v���C���[���W
	static XMFLOAT3 playerPos;

protected:

	//�I�u�W�F�N�g
	std::unique_ptr<Object3d> object;
	//�����t���O
	bool isAlive;
	//HP
	int hp;
	//���W
	XMFLOAT3 pos;
	//�ړ��x�N�g��
	XMFLOAT3 moveVec;
	//�ړ����x
	float speed;
	//�傫��
	float scale;
	//�n�ʂɂ��Ă��邩
	bool onGround;

public:

	bool GetIsAlive() { return isAlive; }
	XMFLOAT3 GetPosition() { return pos; }
	float GetScale() { return scale; }
	void SetIsAlive(bool _isAlive) { isAlive = _isAlive; }
	void Damage() { 
		hp--;
		isAlive = hp != 0;
	}
};