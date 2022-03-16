#include "PostEffect.hlsli"

Texture2D<float4> tex0:register(t0);//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex1:register(t1);//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp:register(s0);//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	//�e�N�X�`���}�b�s���O
	float4 texcolor0 = tex0.Sample(smp, input.uv);
	float4 texcolor1 = tex1.Sample(smp, input.uv);

	float4 color = texcolor0;
	if (fmod(input.uv.y, 0.1) < 0.05f)
	{
		color = texcolor1;
	}

	return float4(color.rgb, 1);
}