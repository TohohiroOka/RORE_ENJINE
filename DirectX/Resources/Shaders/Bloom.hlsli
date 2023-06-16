cbuffer cbuff0 : register(b0)
{
    float strength; //アウトラインの色
};

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};