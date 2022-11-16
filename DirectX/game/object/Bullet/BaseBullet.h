#pragma once
#include "InstanceObject.h"
#include "Emitter.h"

class BaseBullet
{
protected: // �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;

	enum class COLOR {
		RED,//1,0,0
		YELLOW,//1,1,0
		GREEN,//0,1,0
		LIGHT_BLUE,//0,1,1
		BLUE,//0,0,1
		PURPLE,//1,0,1
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
	/// �ÓI�X�V
	/// </summary>
	static void StaticUpdate();

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	static void Draw();

	static void EffectDraw();

	/// <summary>
	/// delete����
	/// </summary>
	static void Finalize();

protected:

	//���f��
	static std::unique_ptr<Model> model;
	//�I�u�W�F�N�g�̍ő�l
	static const int object_max_num = 20;
	//�I�u�W�F�N�g�̍ő�l
	static const int effect_max_num = 20 * 10;
	//�I�u�W�F�N�g
	static std::array<std::unique_ptr<InstanceObject>, object_max_num> bullet;
	//���݂̎g�p��
	static int usingNum;
	//
	static std::array<std::unique_ptr<Emitter>, effect_max_num> effect;
	static std::array<int, effect_max_num> effectNum;

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

