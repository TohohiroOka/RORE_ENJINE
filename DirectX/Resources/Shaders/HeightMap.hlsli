cbuffer cbuff0 : register(b0)
{
	matrix viewproj; // �r���[�v���W�F�N�V�����s��
	matrix world; // ���[���h�s��
	float3 cameraPos; // �J�������W�i���[���h���W�j
};

// ���s�����̐�
static const int DIRLIGHT_NUM = 3;

struct DirLight
{
	float3 lightv; // ���C�g�ւ̕����̒P�ʃx�N�g��
	float3 lightcolor; // ���C�g�̐F(RGB)
	uint active;
};

// �_�����̐�
static const int POINTLIGHT_NUM = 3;

struct PointLight
{
	float3 lightpos;    // ���C�g���W
	float3 lightcolor;  // ���C�g�̐F(RGB)
	float3 lightatten;	// ���C�g���������W��
	uint active;
};

// �X�|�b�g���C�g�̐�
static const int SPOTLIGHT_NUM = 3;

struct SpotLight
{
	float3 lightv;		// ���C�g�̌��������̋t�x�N�g���i�P�ʃx�N�g���j
	float3 lightpos;    // ���C�g���W
	float3 lightcolor;  // ���C�g�̐F(RGB)
	float3 lightatten;	// ���C�g���������W��
	float2 lightfactoranglecos; // ���C�g�����p�x�̃R�T�C��
	uint active;
};

// �ۉe�̐�
static const int CIRCLESHADOW_NUM = 3;

struct CircleShadow
{
	float3 dir;		// ���e�����̋t�x�N�g���i�P�ʃx�N�g���j
	float3 casterPos;    // �L���X�^�[���W
	float  distanceCasterLight;	// �L���X�^�[�ƃ��C�g�̋���
	float3 atten;	// ���������W��
	float2 factorAngleCos; // �����p�x�̃R�T�C��
	uint active;
};

cbuffer cbuff2 : register(b2)
{
	float3 ambientColor;
	DirLight dirLights[DIRLIGHT_NUM];
	PointLight pointLights[POINTLIGHT_NUM];
	SpotLight spotLights[SPOTLIGHT_NUM];
	CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 pos : POSITION0; // �V�X�e���p���_���W
	float4 svpos : SV_POSITION; // �V�X�e���p���_���W
	float4 worldpos : POSITION1; // ���[���h
	float4 color : COLOR;//�F
};