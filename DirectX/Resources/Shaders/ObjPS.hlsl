#include "Obj.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

PSOutput main(VSOutput input)
{
	PSOutput output;

	// テクスチャマッピング
	// テクスチャマッピング
	float difference = 5.0f;
	float4 texcolor = tex.Sample(smp, input.uv);
	float4 bloomColor[4];
	for (int i = 0; i < 4; i++)
	{
		bloomColor[i] = tex.Sample(smp, input.uv / difference * (i + 1));
	}
	// 光沢度
	const float shininess = 4.0f;

	// 頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// 環境反射光
	float3 ambient = m_ambient;

	// シェーディングによる色
	float4 shadecolor = float4(ambientColor * ambient, m_alpha);

	for (int i = 0; i < DIRLIGHT_NUM; i++)
	{
		if (dirLights[i].active)
		{
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);

			//トゥーンシェード
			dotlightnormal.x = step(0.5, dotlightnormal.x);
			dotlightnormal.y = step(0.5, dotlightnormal.y);
			dotlightnormal.z = step(0.5, dotlightnormal.z);

			// 反射光ベクトル
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// 全て加算する
			shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
		}
	}

	// シェーディングによる色で描画
	float4 mainColor = shadecolor * texcolor * color;
	for (int i = 0; i < 4; i++)
	{
		bloomColor[i] = shadecolor * bloomColor[i] * isBloom * color;
	}
	output.target0 = float4(mainColor.rgb, color.w);
	output.target1 = float4(bloomColor[0].rgb, color.w);
	output.target2 = float4(bloomColor[1].rgb, color.w);
	output.target3 = float4(bloomColor[2].rgb, color.w);
	output.target4 = float4(bloomColor[3].rgb, color.w);

	return output;
}