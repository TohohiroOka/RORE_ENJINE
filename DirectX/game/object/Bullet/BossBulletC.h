#pragma once
#include "BaseBullet.h"

class BossBulletC :
    public BaseBullet
{
private:

	enum class EASING {
		LERP,
		IN_QUART,
		OUT_QUART,
		IN_EXPO,
		OUT_EXPO,
		IN_CIRC,
		OUT_CIRC,
		SIZE,
	};

public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_targetPos">�^�[�Q�b�g���W</param>
	BossBulletC(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos);
	~BossBulletC() {};

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_targetPos">�^�[�Q�b�g���W</param>
	/// <param name="_color">�F</param>
	/// <returns></returns>
	static std::unique_ptr<BossBulletC> Create(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, const XMFLOAT3& _color);

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_moveVec">�ړ���</param>
	void Initialize() override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �g���C�[�W���O�̐U�蕪��
	/// </summary>
	/// <param name="kind">�C�[�W���O�ԍ�</param>
	/// <param name="_startPos">�X�^�[�g���W</param>
	/// <param name="_targetPos">�^�[�Q�b�g���W</param>
	/// <param name="rate">�i�݋</param>
	/// <returns>���W</returns>
	float EasingDivide(EASING kind, float _startPos, float _targetPos, float rate);

private:

	int timer;
	XMFLOAT3 startPos;
	XMFLOAT3 targetPos;
	EASING easingKind[3];
	//�ێ����s�����̃t���O
	bool isStorage;
	//�C�[�W���O�ȍ~�̓��������߂邽�߂̕ێ����W
	XMFLOAT3 storagePos;

	const float countPerMeter = 1.0f;
	const float speed = 1.0f;
	const float curvatureRadius = 1.0f;
	const float damping = 0.1f;
	const float impact = 1.0f;
	float maxCentripetalAccel;
	float propulsion; // ���i��
};

