#include "CubeBox.hlsli"

VSOutput main(float4 pos:POSITION, float2 uv : TEXCOORD)
{
	// �s�N�Z���V�F�[�_�[�ɓn���l
	VSOutput output;
	output.svpos = mul(mul(viewproj, world), pos);
	output.uv = uv;

	return output;
}