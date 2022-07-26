#include "DrawLine3D.hlsli"

VSOutput main( float4 pos : POSITION ) : SV_POSITION
{
	VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
	output.svpos = mul(mul(viewproj, world), pos);

	return output;
}