cbuffer cbuff0 : register(b0)
{
	matrix viewproj; // �r���[�v���W�F�N�V�����s��
	matrix world; // ���[���h�s��
	float3 cameraPos; // �J�������W�i���[���h���W�j
	uint isSkinning;//�X�L�j���O
	uint isBloom;//�u���[���̗L��
	uint isToon;//�g�D�[���̗L��
	uint isOutline;//�A�E�g���C���̗L��
};

struct VSOutput
{
	float4 svpos:SV_POSITION;
};