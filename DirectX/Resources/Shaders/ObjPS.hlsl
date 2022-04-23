#include "Obj.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

/// <summary>
/// ブルームのセット
/// </summary>
float4 SetBloom(float4 shadecolor, float4 texcolor, float4 color);

/// <summary>
/// リムライトのセット
/// </summary>
float4 SetLimLight(float4 texcolor, float4 color);


PSOutput main(VSOutput input)
{
	PSOutput output;

	// テクスチャマッピング
	float4 texcolor = tex.Sample(smp, input.uv);

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

	//ブルーム処理
	if (isBloom)
	{
		output.target1 = SetBloom(shadecolor, texcolor, color);
	}

	//リムライト


	////トゥーンシェード
	//shadecolor.x = step(0.4, shadecolor.r);
	//shadecolor.y = step(0.4, shadecolor.g);
	//shadecolor.z = step(0.4, shadecolor.b);


	// シェーディングによる色で描画
	float4 mainColor = shadecolor * texcolor * color;
	output.target0 = float4(mainColor.rgb, color.w);

	return output;
}

float4 SetBloom(float4 shadecolor, float4 texcolor, float4 color)
{
	//光度値の抽出
	float LuminousIntensity = dot(shadecolor.rgb * texcolor.rgb, float3(0.2125, 0.7154, 0.0712));
	
	//ブルームをかける場所
	float4 bloomColor = step(1.0, LuminousIntensity) * texcolor * color;

	////一定値以下ならブルームをかけない
	//bloomColor.r = step(0.2, bloomColor.r) * bloomColor.r;
	//bloomColor.g = step(0.2, bloomColor.g) * bloomColor.g;
	//bloomColor.b = step(0.2, bloomColor.b) * bloomColor.b;

	return bloomColor;
}

//float4 SetLimLight(float4 texcolor, float4 color)
//{
//	float3 EyeDir;   // Uniform 視線ベクトル
//	float3 LightDir; // Uniform ライトベクトル
//
//	// 視線ベクトル
//	float3 eye_vector = normalize(EyeDir);
//	// ライトベクトル
//	float3 light_vector = normalize(LightDir);
//	// 法線
//	float3 normal = normalize(Normal);
//	// リムライトの強さ
//	float rim_power = 2.0;
//
//	// 内積の値を反転しモデルの縁部分が1.0、視線と平行部分を0.0にする。
//	float rim = 1.0 - dot(normal, eye_vector);
//
//	// リムライトの強さを補正
//	rim = pow(abs(rim), rim_power);
//
//	// 太陽との逆光を計算（頂点シェーダでOK）
//	float light_rim = max(dot(-light_vector, eye_vector), 0.0);
//
//	// モデルの縁計算と、太陽の逆光計算を乗算
//	float3 color = rim * light_rim;
//}
