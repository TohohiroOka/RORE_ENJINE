#include "Fbx.hlsli"

//スキニング後の頂点・法線が入る
struct SkinOutput
{
	float4 pos;
	float3 normal;
};

//スキニングの計算
SkinOutput ComputeSkin(VSInput input)
{
	//ゼロクリア
	SkinOutput output = (SkinOutput)0;

	uint iBone;//計算するボーン番号
	float weight;//ボーンの重み
	matrix m;//スキニング行列

	//ボーン0
	iBone = input.boneIndices.x;
	weight = input.boneWeights.x;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);

	//ボーン1
	iBone = input.boneIndices.y;
	weight = input.boneWeights.y;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);

	//ボーン2
	iBone = input.boneIndices.z;
	weight = input.boneWeights.z;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);

	//ボーン3
	iBone = input.boneIndices.w;
	weight = input.boneWeights.w;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);

	//iBone = input.boneIndices.x;
	//m = matSkinning[iBone];
	//output.pos = mul(m, input.pos);
	//output.normal = mul((float3x3)m, input.normal);

	return output;
}

VSOutput main(VSInput input)
{
	//スキニング計算
	SkinOutput skinned = ComputeSkin(input);
	// 法線にワールド行列によるスケーリング・回転を適用
	float4 wnormal = normalize(mul(world, float4(skinned.normal, 0)));

	// ピクセルシェーダーに渡す値
	VSOutput output;
	output.svpos = mul(mul(viewproj, world), skinned.pos);
	output.worldpos = mul(world, skinned.pos);
	output.normal = wnormal.xyz;
	output.uv = input.uv;

	return output;
}