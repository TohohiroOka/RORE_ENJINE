cbuffer cbuff0:register(b0)
{
	float4 outlineColor;//�A�E�g���C���̐F
	matrix a;
};

struct VSOutput
{
	float4 svpos:SV_POSITION;
	float2 uv : TEXCOORD;
};