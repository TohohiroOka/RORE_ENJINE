#include "Obj.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

PSOutput main(VSOutput input)
{
	PSOutput output;

	// テクスチャマッピング
	float4 texcolor = tex.Sample(smp, input.uv);
	float4 bloomColor;
	float difference[2];//ずらす量
	difference[0] = lerp(0.0f, 5.0f, input.svpos.x);
	difference[1] = lerp(0.0f, 5.0f, input.svpos.y);
	bloomColor = tex.Sample(smp, (input.uv.x + difference[0], input.uv.y + difference[1])) / 2;
	bloomColor += tex.Sample(smp, (input.uv.x + difference[0], input.uv.y - difference[1])) / 2;
	bloomColor += tex.Sample(smp, (input.uv.x - difference[0], input.uv.y + difference[1])) / 2;
	bloomColor += tex.Sample(smp, (input.uv.x - difference[0], input.uv.y - difference[1])) / 2;

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
			// 反射光ベクトル
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// 全て加算する
			shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;

			//トゥーンシェード
			shadecolor.x = step(0.5, shadecolor.x);
			shadecolor.y = step(0.5, shadecolor.y);
			shadecolor.z = step(0.5, shadecolor.z);
		}
	}

	// シェーディングによる色で描画
	float4 mainColor = shadecolor * texcolor * color;
	bloomColor = shadecolor * bloomColor * isBloom * color;

	output.target0 = float4(mainColor.rgb, color.w);
	output.target1 = float4(bloomColor.rgb, color.w);

	return output;
}