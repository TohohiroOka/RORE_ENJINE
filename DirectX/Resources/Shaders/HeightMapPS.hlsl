#include "HeightMap.hlsli"

Texture2D<float4> tex : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	float4 color = input.color;

	float3 m_ambient = float3(0.5, 0.5, 0.5);
	float3 m_diffuse = float3(0.5, 0.5, 0.5);
	float3 m_specular = float3(0.5, 0.5, 0.5);
	float m_alpha = 1.0f;

	float3 normal = input.svpos.xyz;

	// ����x
	const float shininess = 4.0f;

	// ���_���王�_�ւ̕����x�N�g��
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// �����ˌ�
	float3 ambient = m_ambient;

	// �V�F�[�f�B���O�ɂ��F
	float4 shadecolor = float4(ambientColor * ambient, m_alpha);

	//���s����
	for (int i = 0; i < DIRLIGHT_NUM; i++)
	{
		if (dirLights[i].active)
		{
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
			float3 dotlightnormal = dot(dirLights[i].lightv, normal);

			// ���ˌ��x�N�g��
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * normal);
			// �g�U���ˌ�
			float3 diffuse = dotlightnormal * m_diffuse;
			// ���ʔ��ˌ�
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// �S�ĉ��Z����
			shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
		}
	}

	//�_����
	for (i = 0; i < POINTLIGHT_NUM; i++)
	{
		if (pointLights[i].active)
		{
			//���C�g�ւ̃x�N�g��
			float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			//�x�N�g���̒���
			float d = length(lightv);
			//���K�����A�P�ʃx�N�g���ɂ���
			lightv = normalize(lightv);
			//���������W��
			float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z * d * d);
			//���C�g�Ɍ������x�N�g���Ɩ@���̓���
			float3 dotlightnormal = dot(lightv, normal);
			//���ˌ��x�N�g��
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * normal);
			// �g�U���ˌ�
			float3 diffuse = dotlightnormal * m_diffuse;
			// ���ʔ��ˌ�
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// �S�ĉ��Z����
			shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor;
		}
	}

	// �X�|�b�g���C�g
	for (i = 0; i < SPOTLIGHT_NUM; i++)
	{
		if (spotLights[i].active)
		{
			// ���C�g�ւ̕����x�N�g��
			float3 lightv = spotLights[i].lightpos - input.worldpos.xyz;
			//�x�N�g���̒���
			float d = length(lightv);
			//���K�����A�P��
			lightv = normalize(lightv);
			// ���������W��
			float atten = saturate(1.0f / (spotLights[i].lightatten.x + spotLights[i].lightatten.y * d + spotLights[i].lightatten.z * d * d));
			// �p�x����
			float cos = dot(lightv, spotLights[i].lightv);
			// �����J�n�p�x����A�����I���p�x�ɂ����Č���
			// �����J�n�p�x�̓�����1�{ �����I���p�x�̊O����0�{�̋P�x
			float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos);
			// �p�x��������Z
			atten *= angleatten;
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
			float3 dotlightnormal = dot(lightv, normal);
			// ���ˌ��x�N�g��
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * normal);
			// �g�U���ˌ�
			float3 diffuse = dotlightnormal * m_diffuse;
			// ���ʔ��ˌ�
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// �S�ĉ��Z����
			shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].lightcolor;
		}
	}

	// �ۉe
	for (i = 0; i < CIRCLESHADOW_NUM; i++)
	{
		if (circleShadows[i].active)
		{
			// �I�u�W�F�N�g�\�ʂ���L���X�^�[�ւ̃x�N�g��
			float3 casterv = circleShadows[i].casterPos - input.worldpos.xyz;
			// ���������ł̋���
			float d = dot(casterv, circleShadows[i].dir);
			// ���������W��
			float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));
			// �������}�C�i�X�Ȃ�0�ɂ���
			atten *= step(0, d);
			// ���C�g�̍��W
			float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;
			//  �I�u�W�F�N�g�\�ʂ��烉�C�g�ւ̃x�N�g���i�P�ʃx�N�g���j
			float3 lightv = normalize(lightpos - input.worldpos.xyz);
			// �p�x����
			float cos = dot(lightv, circleShadows[i].dir);
			// �����J�n�p�x����A�����I���p�x�ɂ����Č���
			// �����J�n�p�x�̓�����1�{ �����I���p�x�̊O����0�{�̋P�x
			float angleatten = smoothstep(circleShadows[i].factorAngleCos.y, circleShadows[i].factorAngleCos.x, cos);
			// �p�x��������Z
			atten *= angleatten;

			// �S�Č��Z����
			shadecolor.rgb -= atten;
		}
	}

	float4 mainColor = shadecolor * color;

	return color;
	//return float4(1, 1, 1, 1);
}