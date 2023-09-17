#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float4> bloom : register(t1); //1番スロットに設定されたテクスチャ
Texture2D<float4> outline : register(t2); //2番スロットに設定されたテクスチャ
Texture2D<float4> fog : register(t3); //2番スロットに設定されたテクスチャ
Texture2D<float> depthTex : register(t4); //1番スロットに設定されたテクスチャ
SamplerState smp:register(s0);//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//メインカラー
	float4 mainColor = tex.Sample(smp, input.uv);
	
	//bloom
	float4 bloomColor = bloom.Sample(smp, input.uv);

	//bloom
	float4 outlineColor = outline.Sample(smp, input.uv);
	
	//fog
	float4 fogColor = fog.Sample(smp, input.uv);

	//ポストエフェクトの合成
    return mainColor + bloomColor + outlineColor + fogColor;
}