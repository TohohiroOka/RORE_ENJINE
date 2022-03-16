cbuffer cbuff0:register(b0)
{
	float4 color;//色
	matrix world;//3D変換行列
	matrix view;//3D変換行列
	matrix projection;//3D変換行列
};

struct VSOutput
{
	float4 svpos:SV_POSITION;
};