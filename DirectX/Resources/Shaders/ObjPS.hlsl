#include "Obj.hlsli"

Texture2D<float4> tex : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

PSOutput main(VSOutput input)
{
	PSOutput output;

	// �e�N�X�`���}�b�s���O
	// �e�N�X�`���}�b�s���O
	float difference = 5.0f;
	float4 texcolor = tex.Sample(smp, input.uv);
	float4 bloomColor[4];
	for (int i = 0; i < 4; i++)
	{
		bloomColor[i] = tex.Sample(smp, input.uv / difference * (i + 1));
	}
	// ����x
	const float shininess = 4.0f;

	// ���_���王�_�ւ̕����x�N�g��
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// �����ˌ�
	float3 ambient = m_ambient;

	// �V�F�[�f�B���O�ɂ��F
	float4 shadecolor = float4(ambientColor * ambient, m_alpha);

	for (int i = 0; i < DIRLIGHT_NUM; i++)
	{
		if (dirLights[i].active)
		{
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
			float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);

			//�g�D�[���V�F�[�h
			dotlightnormal.x = step(0.5, dotlightnormal.x);
			dotlightnormal.y = step(0.5, dotlightnormal.y);
			dotlightnormal.z = step(0.5, dotlightnormal.z);

			// ���ˌ��x�N�g��
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
			// �g�U���ˌ�
			float3 diffuse = dotlightnormal * m_diffuse;
			// ���ʔ��ˌ�
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// �S�ĉ��Z����
			shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
		}
	}

	// �V�F�[�f�B���O�ɂ��F�ŕ`��
	float4 mainColor = shadecolor * texcolor * color;
	for (int i = 0; i < 4; i++)
	{
		bloomColor[i] = shadecolor * bloomColor[i] * isBloom * color;
	}
	output.target0 = float4(mainColor.rgb, color.w);
	output.target1 = float4(bloomColor[0].rgb, color.w);
	output.target2 = float4(bloomColor[1].rgb, color.w);
	output.target3 = float4(bloomColor[2].rgb, color.w);
	output.target4 = float4(bloomColor[3].rgb, color.w);

	return output;
}