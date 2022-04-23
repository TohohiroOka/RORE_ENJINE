#include "PostEffect.hlsli"

Texture2D<float4> tex0:register(t0);//0番スロットに設定されたテクスチャ
Texture2D<float4> tex1:register(t1);//1番スロットに設定されたテクスチャ
SamplerState smp:register(s0);//0番スロットに設定されたサンプラー

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
	float d = distance(drawUV, pickUV);
	return exp(-d * d) / (2 * sigma * sigma);
}

float4 main(VSOutput input) : SV_TARGET
{
	//---------bloom処理----------//
	float totalWeight = 0;
	float sigma = 0.005;
	float stepWidth = 0.001;
	float4 col = { 0, 0, 0, 0 };
	float4 MainTex = tex0.Sample(smp, input.uv);

	for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth)
	{
		for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth)
		{
			float2 pickUV = input.uv + float2(px, py);
			float weight = Gaussian(input.uv, pickUV, sigma);
			col += tex1.Sample(smp, pickUV) * weight;
			totalWeight += weight;
		}
	}

	col.rgb = col.rgb / totalWeight;

	return float4(MainTex.rgb + col.rgb, 1.0f);
}