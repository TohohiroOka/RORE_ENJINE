cbuffer cbuff0 : register(b0)
{
	matrix viewproj; // ビュープロジェクション行列
	matrix world; // ワールド行列
	float3 cameraPos; // カメラ座標（ワールド座標）
	uint isSkinning;//スキニング
	uint isBloom;//ブルームの有無
	uint isToon;//トゥーンの有無
	uint isOutline;//アウトラインの有無
};

struct VSOutput
{
	float4 svpos:SV_POSITION;
};