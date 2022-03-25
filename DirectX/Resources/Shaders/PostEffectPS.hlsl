#include "PostEffect.hlsli"

Texture2D<float4> tex0:register(t0);//0番スロットに設定されたテクスチャ
Texture2D<float4> tex1:register(t1);//1番スロットに設定されたテクスチャ
Texture2D<float4> tex2:register(t2);//2番スロットに設定されたテクスチャ
Texture2D<float4> tex3:register(t3);//3番スロットに設定されたテクスチャ
Texture2D<float4> tex4:register(t4);//3番スロットに設定されたテクスチャ
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
			col += tex2.Sample(smp, pickUV) * weight;
			col += tex3.Sample(smp, pickUV) * weight;
			col += tex4.Sample(smp, pickUV) * weight;
			totalWeight += weight;
		}
	}

	col.rgb = col.rgb / totalWeight;

	//float4 MainTex = tex0.Sample(smp, input.uv);
	//float4 color1 = tex1.Sample(smp, input.uv);
	//float4 color2 = tex2.Sample(smp, input.uv);
	//float4 color3 = tex3.Sample(smp, input.uv);
	//float4 color4 = tex4.Sample(smp, input.uv);

	return float4(MainTex.rgb + col.rgb, 1.0f);
}