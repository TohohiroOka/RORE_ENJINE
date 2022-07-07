#include "HeightMap.hlsli"

VSOutput main(float4 pos : POSITION,float2 uv : TEXCOORD)
{
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = pos;
	output.svpos = mul(mul(viewproj, world), pos);
	output.worldpos = mul(world, pos);
	output.uv = uv;
	//output.color = float4(output.worldpos.y / 1000, output.worldpos.y / 1000, output.worldpos.y / 1000, 1.0);
	return output;
}