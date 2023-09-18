#include "HeightMap.hlsli"

Texture2D<float4> heightTex : register(t0);  // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex : register(t1);  // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex2 : register(t2);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

/// <summary>
/// ブルームのセット
/// </summary>
float4 SetBloom(float4 shadecolor, float4 texcolor);

/// <summary>
/// トゥーンのセット
/// </summary>
float4 SetToon(float4 shadecolor);

PSOutput main(VSOutput input) : SV_TARGET
{
    float4 m_baseColor = float4(baseColor.rgb, 1.0);
    float3 m_ambient = float3(0.5, 0.5, 0.5);
    float3 m_diffuse = float3(0.5, 0.5, 0.5);
    float3 m_specular = float3(0.5, 0.5, 0.5);
    float m_alpha = 1.0f;

	// 光沢度
	const float shininess = 4.0f;

	// 頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// 環境反射光
	float3 ambient = m_ambient;

	// シェーディングによる色
	float4 shadecolor = float4(ambientColor * ambient, m_alpha);

	//平行光源
	for (int i = 0; i < DIRLIGHT_NUM; i++)
	{
		if (dirLights[i].active)
		{
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);

			// 反射光ベクトル
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// 全て加算する
			shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
		}
	}

	//点光源
	for (i = 0; i < POINTLIGHT_NUM; i++)
	{
		if (pointLights[i].active)
		{
			//ライトへのベクトル
			float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			//ベクトルの長さ
			float d = length(lightv);
			//正規化し、単位ベクトルにする
			lightv = normalize(lightv);
			//距離減衰係数
			float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z * d * d);
			//ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(lightv, input.normal);
			//反射光ベクトル
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// 全て加算する
			shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor;
		}
	}

	// スポットライト
	for (i = 0; i < SPOTLIGHT_NUM; i++)
	{
		if (spotLights[i].active)
		{
			// ライトへの方向ベクトル
			float3 lightv = spotLights[i].lightpos - input.worldpos.xyz;
			//ベクトルの長さ
			float d = length(lightv);
			//正規化し、単位
			lightv = normalize(lightv);
			// 距離減衰係数
			float atten = saturate(1.0f / (spotLights[i].lightatten.x + spotLights[i].lightatten.y * d + spotLights[i].lightatten.z * d * d));
			// 角度減衰
			float cos = dot(lightv, spotLights[i].lightv);
			// 減衰開始角度から、減衰終了角度にかけて減衰
			// 減衰開始角度の内側は1倍 減衰終了角度の外側は0倍の輝度
			float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos);
			// 角度減衰を乗算
			atten *= angleatten;
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// 全て加算する
			shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].lightcolor;
		}
	}

	// 丸影
	for (i = 0; i < CIRCLESHADOW_NUM; i++)
	{
		if (circleShadows[i].active)
		{
			// オブジェクト表面からキャスターへのベクトル
			float3 casterv = circleShadows[i].casterPos - input.worldpos.xyz;
			// 光線方向での距離
			float d = dot(casterv, circleShadows[i].dir);
			// 距離減衰係数
			float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));
			// 距離がマイナスなら0にする
			atten *= step(0, d);
			// ライトの座標
			float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;
			//  オブジェクト表面からライトへのベクトル（単位ベクトル）
			float3 lightv = normalize(lightpos - input.worldpos.xyz);
			// 角度減衰
			float cos = dot(lightv, circleShadows[i].dir);
			// 減衰開始角度から、減衰終了角度にかけて減衰
			// 減衰開始角度の内側は1倍 減衰終了角度の外側は0倍の輝度
			float angleatten = smoothstep(circleShadows[i].factorAngleCos.y, circleShadows[i].factorAngleCos.x, cos);
			// 角度減衰を乗算
			atten *= angleatten;

			// 全て減算する
			shadecolor.rgb -= atten;
		}
	}

	float4 color1 = tex.Sample(smp, input.uv);
	float4 color2 = tex2.Sample(smp, input.uv);

	//float stepNum1 = 2.5;
	//float stepNum2 = 3.5;
	float stepNum1 = 2.0;
	float stepNum2 = 3.0;
	float4 addColor = smoothstep(stepNum1, stepNum2, input.localpos.y) * color1 + (1 - smoothstep(stepNum1, stepNum2, input.localpos.y)) * color2;

	//ブルーム処理
    float4 bloom = float4(0, 0, 0, 0);
    if (isBloom)
    {
        bloom = SetBloom(shadecolor, addColor);
        bloom = float4(bloom.rgb, 0.1);
    }

	//トゥーンシェード
    if (isToon)
    {
        shadecolor = SetToon(shadecolor);
    }


	// シェーディングの濃さ調整
    shadecolor.x += step(shadecolor.x, 0.4) + 0.4;
    shadecolor.y += step(shadecolor.y, 0.4) + 0.4;
    shadecolor.z += step(shadecolor.z, 0.4) + 0.4;


	// シェーディングによる色で描画
    PSOutput output;
    float4 mainColor = shadecolor * addColor;
    output.target0 = float4(mainColor.rgb, 1.0);
    output.target1 = bloom;
    output.target2 = float4(outlineColor, 1.0f) * isOutline;

    return output;
}

float4 SetBloom(float4 shadecolor, float4 texcolor)
{
	//光度値の抽出
    float LuminousIntensity = dot(shadecolor.rgb * texcolor.rgb, float3(0.2125, 0.7154, 0.0712));

	//ブルームをかける場所
    float4 bloomColor = step(1.0, LuminousIntensity) * texcolor;

    return bloomColor;
}

float4 SetToon(float4 shadecolor)
{
	//トゥーンの色範囲
    float toonLighrRange = 0.525;
	//明暗の中間幅
    float contourWidth = 0.1;

	//明るい
    float3 bright;
    bright.r = step(toonLighrRange, shadecolor.r);
    bright.g = step(toonLighrRange, shadecolor.g);
    bright.b = step(toonLighrRange, shadecolor.b);
	//暗い
    float3 dark;
    dark.r = (1 - step(toonLighrRange, shadecolor.r));
    dark.g = (1 - step(toonLighrRange, shadecolor.g));
    dark.b = (1 - step(toonLighrRange, shadecolor.b));

	//中間
    float3 intermediate;
    intermediate.r = smoothstep(toonLighrRange, toonLighrRange + contourWidth, shadecolor.r);
    intermediate.g = smoothstep(toonLighrRange, toonLighrRange + contourWidth, shadecolor.g);
    intermediate.b = smoothstep(toonLighrRange, toonLighrRange + contourWidth, shadecolor.b);

	//現在の色
    return float4(bright + dark + intermediate, 1);
}