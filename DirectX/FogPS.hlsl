#include "Fog.hlsli"

Texture2D<float> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main() : SV_TARGET
{
    float tex = abs(tex.Sample(smp, uv));
    float depth = pow(tex, 10000);
    depth = depth * depth;
    float4 color = float4(0, depth / 2, depth, 1.0);

    return color;
}