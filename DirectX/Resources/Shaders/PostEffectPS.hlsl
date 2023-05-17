#include "PostEffect.hlsli"

Texture2D<float4> tex:register(t0);//0番スロットに設定されたテクスチャ
SamplerState smp:register(s0);//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//メインカラー
	float4 mainColor = tex.Sample(smp, input.uv);
	//ポストエフェクトの合成
	return mainColor;
}