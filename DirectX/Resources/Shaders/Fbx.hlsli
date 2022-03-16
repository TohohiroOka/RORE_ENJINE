cbuffer cbuff0 : register(b0)
{
	matrix viewproj;//ビュープロジェクション行列
	matrix world;//ワールド行列
	float3 cameraPos;//カメラ座標（ワールド座標）
	float4 color;//色
};

//ボーンの最大値
static const int MAX_BONES = 32;
cbuffer skinning : register(b3)
{
	matrix matSkinning[MAX_BONES];
};

//バーテックスバッファーの入力
struct VSInput
{
	float4 pos	: POSITION;//位置
	float3 normal : NORMAL;//頂点法線
	float2 uv	: TEXCOORD;//テクスチャー座標
	uint4  boneIndices : BONEINDICES;//ボーンの番号
	float4 boneWeights : BONEWEIGHTS;//ボーンのスキンウェイト
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
	float3 normal : NORMAL; //法線ベクトル
	float2 uv  :TEXCOORD; // uv値
};

struct PSOutput
{
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};
