#include "BasicShaderHeader.hlsli"

VSOutput main(float4 pos:POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD,
	float3 tangent : TANGENT, float3 binormal : BINORMAL)
{
	VSOutput output;//�s�N�Z���V�F�[�_�[�ɓn���l
	////�s���pos�ɔ��f
	output.svpos = mul(world, pos);
	output.svpos = mul(view, output.svpos);
	output.svpos = mul(projection, output.svpos);

	//uv�𔽉f
	output.uv = uv;

	//�@���ƍs��̌v�Z
	output.normal = mul(world, normal);
	output.normal = normalize(output.normal);

	//
	output.tangent = mul(world, tangent);
	output.tangent = normalize(output.tangent);

	//
	output.binormal = mul(world, binormal);
	output.binormal = normalize(output.binormal);

	return output;
}