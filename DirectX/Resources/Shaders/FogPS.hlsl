#include "Fog.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    float absTex = abs(tex.Sample(smp, input.uv));
    float depth = pow(absTex, 10000);
    depth = depth * depth * strength;
    float3 outColor = color * depth;

    return float4(outColor.rgb, 1.0f * depth);
}