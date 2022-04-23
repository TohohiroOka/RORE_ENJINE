#include "Obj.hlsli"

Texture2D<float4> tex : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

/// <summary>
/// �u���[���̃Z�b�g
/// </summary>
float4 SetBloom(float4 shadecolor, float4 texcolor, float4 color);

/// <summary>
/// �������C�g�̃Z�b�g
/// </summary>
float4 SetLimLight(float4 texcolor, float4 color);


PSOutput main(VSOutput input)
{
	PSOutput output;

	// �e�N�X�`���}�b�s���O
	float4 texcolor = tex.Sample(smp, input.uv);

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
			float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);

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
			float3 dotlightnormal = dot(lightv, input.normal);
			//���ˌ��x�N�g��
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
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
			float3 dotlightnormal = dot(lightv, input.normal);
			// ���ˌ��x�N�g��
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
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

	//�u���[������
	if (isBloom)
	{
		output.target1 = SetBloom(shadecolor, texcolor, color);
	}

	//�������C�g


	////�g�D�[���V�F�[�h
	//shadecolor.x = step(0.4, shadecolor.r);
	//shadecolor.y = step(0.4, shadecolor.g);
	//shadecolor.z = step(0.4, shadecolor.b);


	// �V�F�[�f�B���O�ɂ��F�ŕ`��
	float4 mainColor = shadecolor * texcolor * color;
	output.target0 = float4(mainColor.rgb, color.w);

	return output;
}

float4 SetBloom(float4 shadecolor, float4 texcolor, float4 color)
{
	//���x�l�̒��o
	float LuminousIntensity = dot(shadecolor.rgb * texcolor.rgb, float3(0.2125, 0.7154, 0.0712));
	
	//�u���[����������ꏊ
	float4 bloomColor = step(1.0, LuminousIntensity) * texcolor * color;

	////���l�ȉ��Ȃ�u���[���������Ȃ�
	//bloomColor.r = step(0.2, bloomColor.r) * bloomColor.r;
	//bloomColor.g = step(0.2, bloomColor.g) * bloomColor.g;
	//bloomColor.b = step(0.2, bloomColor.b) * bloomColor.b;

	return bloomColor;
}

//float4 SetLimLight(float4 texcolor, float4 color)
//{
//	float3 EyeDir;   // Uniform �����x�N�g��
//	float3 LightDir; // Uniform ���C�g�x�N�g��
//
//	// �����x�N�g��
//	float3 eye_vector = normalize(EyeDir);
//	// ���C�g�x�N�g��
//	float3 light_vector = normalize(LightDir);
//	// �@��
//	float3 normal = normalize(Normal);
//	// �������C�g�̋���
//	float rim_power = 2.0;
//
//	// ���ς̒l�𔽓]�����f���̉�������1.0�A�����ƕ��s������0.0�ɂ���B
//	float rim = 1.0 - dot(normal, eye_vector);
//
//	// �������C�g�̋�����␳
//	rim = pow(abs(rim), rim_power);
//
//	// ���z�Ƃ̋t�����v�Z�i���_�V�F�[�_��OK�j
//	float light_rim = max(dot(-light_vector, eye_vector), 0.0);
//
//	// ���f���̉��v�Z�ƁA���z�̋t���v�Z����Z
//	float3 color = rim * light_rim;
//}
