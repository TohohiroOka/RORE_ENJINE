#include "Particle.hlsli"

Texture2D<float> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(GSOutput input) : SV_TARGET
{
	float4 color = tex.Sample(smp, input.uv);
	color = color * input.color;

	//ブルーム処理
	float4 bloom = float4(0, 0, 0, 0);
	if (isBloom)
	{
		bloom = input.color;
	}


	return float4(color.rgb, input.color.w);
}