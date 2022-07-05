#include "HeightMap.hlsli"

VSOutput main(float4 pos : POSITION)
{
	VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
	output.svpos = mul(mul(viewproj, world), pos);
	output.worldpos = mul(world, pos);
	output.color = float4(output.worldpos.y / 1000, output.worldpos.y / 1000, output.worldpos.y / 1000, 1.0);
	return output;
}