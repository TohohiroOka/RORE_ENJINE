cbuffer cbuff0 : register(b0)
{
	float4 color;
	matrix world; // ワールド行列
	matrix viewproj; // ビュープロジェクション行列
};

struct VSOutput
{
	float4 svpos:SV_POSITION;
};