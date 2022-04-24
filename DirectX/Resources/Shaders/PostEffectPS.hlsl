#include "PostEffect.hlsli"

Texture2D<float4> tex0:register(t0);//0番スロットに設定されたテクスチャ
Texture2D<float4> tex1:register(t1);//1番スロットに設定されたテクスチャ
Texture2D<float4> tex2:register(t2);//1番スロットに設定されたテクスチャ
SamplerState smp:register(s0);//0番スロットに設定されたサンプラー

/// <summary>
/// ガウス処理
/// </summary>
float Gaussian(float2 drawUV, float2 pickUV, float sigma);

/// <summary>
/// ブルーム処理
/// </summary>
float4 SetBloom(float2 uv);

/// <summary>
/// アウトライン処理
/// </summary>
float4 SetOutline(float2 uv, float outlineWidth, float4 outlineColor);

float4 main(VSOutput input) : SV_TARGET
{
	//メインカラー
	float4 mainColor = tex0.Sample(smp, input.uv);

	//bloom処理
	float4 bloom = SetBloom(input.uv);

	//アウトライン処理
	float4 outline = SetOutline(input.uv, outlineWidth, outlineColor);

	//ポストエフェクトの合成
	mainColor = float4(mainColor.rgb + bloom.rgb + outline.rgb, 1.0);

	return mainColor;
}

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
	float d = distance(drawUV, pickUV);
	return exp(-d * d) / (2 * sigma * sigma);
}

float4 SetBloom(float2 uv)
{
	float totalWeight = 0;
	float sigma = 0.005;
	float stepWidth = 0.001;
	float4 color = { 0, 0, 0, 0 };

	for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth)
	{
		for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth)
		{
			float2 pickUV = uv + float2(px, py);
			float weight = Gaussian(uv, pickUV, sigma);
			color += tex1.Sample(smp, pickUV) * weight;
			totalWeight += weight;
		}
	}

	color.rgb = color.rgb / totalWeight;

	return color;
}

float4 SetOutline(float2 uv, float outlineWidth, float4 outlineColor)
{
	float4 outlineTex = float4(0, 0, 0, 0);

	float xPoutline = uv.x + outlineWidth;
	float xMoutline = uv.x - outlineWidth;
	float yPoutline = uv.y + outlineWidth;
	float yMoutline = uv.y - outlineWidth;

	outlineTex = outlineTex + tex2.Sample(smp, float2(xPoutline, yPoutline));
	outlineTex = outlineTex + tex2.Sample(smp, float2(xPoutline, yMoutline));
	outlineTex = outlineTex + tex2.Sample(smp, float2(xMoutline, yPoutline));
	outlineTex = outlineTex + tex2.Sample(smp, float2(xMoutline, yMoutline));
	outlineTex = outlineTex + tex2.Sample(smp, float2(xPoutline, uv.y));
	outlineTex = outlineTex + tex2.Sample(smp, float2(xMoutline, uv.y));
	outlineTex = outlineTex + tex2.Sample(smp, float2(uv.x, yPoutline));
	outlineTex = outlineTex + tex2.Sample(smp, float2(uv.x, yMoutline));

	float4 normalColor = tex2.Sample(smp, uv);
	outlineTex.rgb = outlineTex.rgb - normalColor.rgb * 8.0;

	//カラーの合計値
	float addColor = 0;
	addColor = outlineTex.r + outlineTex.g + outlineTex.b;
	//0.5より大きければアウトライン生成
	addColor = step(0.1, addColor);//0.5より大きければアウトライン生成

	outlineTex.r = outlineColor.r * addColor;
	outlineTex.g = outlineColor.g * addColor;
	outlineTex.b = outlineColor.b * addColor;

	return outlineTex;
}