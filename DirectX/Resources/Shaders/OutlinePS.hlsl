#include "Outline.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

/// <summary>
/// 黒ならそのまま出力
/// </summary>
float4 SetColor(float4 color, float4 nowColor);

float4 main(VSOutput input) : SV_TARGET
{
    float4 outlineTex = float4(0, 0, 0, 0);

    float xPoutline = input.uv.x + outlineWidth;
    float xMoutline = input.uv.x - outlineWidth;
    float yPoutline = input.uv.y + outlineWidth;
    float yMoutline = input.uv.y - outlineWidth;

    float add1 = step(xPoutline, 1);
    float add2 = step(0, xMoutline);
    float add3 = step(yPoutline, 1);
    float add4 = step(0, yMoutline);

    outlineTex = SetColor(tex.Sample(smp, float2(xPoutline, yPoutline)) * (add1 * add3), outlineTex);
    outlineTex = SetColor(tex.Sample(smp, float2(xPoutline, yMoutline)) * (add1 * add4), outlineTex);
    outlineTex = SetColor(tex.Sample(smp, float2(xMoutline, yPoutline)) * (add2 * add3), outlineTex);
    outlineTex = SetColor(tex.Sample(smp, float2(xMoutline, yMoutline)) * (add2 * add4), outlineTex);
    outlineTex = SetColor(tex.Sample(smp, float2(xPoutline, input.uv.y)) * add1, outlineTex);
    outlineTex = SetColor(tex.Sample(smp, float2(xMoutline, input.uv.y)) * add2, outlineTex);
    outlineTex = SetColor(tex.Sample(smp, float2(input.uv.x, yPoutline)) * add3, outlineTex);
    outlineTex = SetColor(tex.Sample(smp, float2(input.uv.x, yMoutline)) * add4, outlineTex);

    float4 normalColor = tex.Sample(smp, input.uv);
    outlineTex.rgb = outlineTex.rgb - normalColor.rgb;
    outlineTex.a = 1.0f;
    
    return outlineTex;
}

float4 SetColor(float4 color, float4 nowColor)
{
    float a = step(0.1, color.x);
    float b = step(0.1, color.y);
    float c = step(0.1, color.z);

    float d = step(1, a + b + c);
    
    return color * d + nowColor * (1 - d);
}
