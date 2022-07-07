#include "HeightMap.hlsli"

VSOutput main(float4 pos : POSITION,float2 uv : TEXCOORD)
{
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = pos;
	output.svpos = mul(mul(viewproj, world), pos);
	output.worldpos = mul(world, pos);
	output.uv = uv;

	return output;
}