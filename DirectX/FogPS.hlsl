#include "Fog.hlsli"

Texture2D<float> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main() : SV_TARGET
{
    float tex = abs(tex.Sample(smp, uv));
    float depth = pow(tex, 10000);
    depth = depth * depth;
    float4 color = float4(0, depth / 2, depth, 1.0);

    return color;
}