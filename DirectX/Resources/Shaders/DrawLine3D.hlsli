cbuffer cbuff0 : register(b0)
{
	float4 color;
	matrix world; // ���[���h�s��
	matrix viewproj; // �r���[�v���W�F�N�V�����s��
};

struct VSOutput
{
	float4 svpos:SV_POSITION;
};