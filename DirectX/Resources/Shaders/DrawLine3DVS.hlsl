#include "DrawLine3D.hlsli"

VSOutput main( float4 pos : POSITION ) : SV_POSITION
{
	VSOutput output; // ピクセルシェーダーに渡す値
	output.svpos = mul(mul(viewproj, world), pos);

	return output;
}