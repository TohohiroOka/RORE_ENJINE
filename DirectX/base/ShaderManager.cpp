#include "ShaderManager.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

const std::wstring pass = L"Resources/Shaders/";

//エラーの出力
void OutputErrstr(ID3DBlob* errorBlob)
{
	// errorBlobからエラー内容をstring型にコピー
	std::string errstr;
	errstr.resize(errorBlob->GetBufferSize());

	std::copy_n((char*)errorBlob->GetBufferPointer(),
		errorBlob->GetBufferSize(),
		errstr.begin());
	errstr += "\n";
	// エラー内容を出力ウィンドウに表示
	OutputDebugStringA(errstr.c_str());
	exit(1);
}

//パスの取得
std::wstring GetFullPath(LPCWSTR assetName)
{
	return pass + assetName;
}

//シェーダーの読み込みとコンパイル
ID3DBlob* CompileShader(const LPCWSTR fileName, LPCSTR target)
{
	HRESULT result = S_FALSE;
	//出力用オブジェクト
	ID3DBlob* blob = nullptr;
	//インクルード設定
	ID3DInclude* includeFrag = D3D_COMPILE_STANDARD_FILE_INCLUDE;
	// デバッグ用設定
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	//エラーオブジェクト
	ID3DBlob* error;

	result = D3DCompileFromFile(GetFullPath(fileName).c_str(), nullptr, includeFrag, "main", target,
		compileFlags, 0, &blob, &error);
	if (FAILED(result))
	{
		OutputErrstr(error);
	}

	return blob;
}

std::unique_ptr<ShaderManager> ShaderManager::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	ShaderManager* instance = new ShaderManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->LoadShader();

	return std::unique_ptr<ShaderManager>(instance);
}

void ShaderManager::LoadShader()
{
	HRESULT result = S_FALSE;

	//頂点シェーダーモデル
	LPCSTR vsModel = "vs_5_0";
	//ピクセルシェーダーモデル
	LPCSTR psModel = "ps_5_0";
	//ジオメトリシェーダーモデル
	LPCSTR gsModel = "gs_5_0";
	//コンピュートシェーダーモデル
	LPCSTR csModel = "cs_5_0";

	//Objシェーダー
	shaderObjectVS["OBJ"] = CompileShader(L"ObjVS.hlsl", vsModel);
	shaderObjectPS["OBJ"] = CompileShader(L"ObjPS.hlsl", psModel);
	//Fbxシェーダー
	shaderObjectVS["FBX"] = CompileShader(L"FbxVS.hlsl", vsModel);
	shaderObjectPS["FBX"] = CompileShader(L"FbxPS.hlsl", psModel);
	//DrawLine3dシェーダー
	shaderObjectVS["DRAW_LINE_3D"] = CompileShader(L"DrawLine3DVS.hlsl", vsModel);
	shaderObjectPS["DRAW_LINE_3D"] = CompileShader(L"DrawLine3DPS.hlsl", psModel);
	//NormalMapシェーダー
	shaderObjectVS["NORMAL_MAP"] = CompileShader(L"NormalMapVS.hlsl", vsModel);
	shaderObjectPS["NORMAL_MAP"] = CompileShader(L"NormalMapPS.hlsl", psModel);
	//Spriteシェーダー
	shaderObjectVS["SPRITE"] = CompileShader(L"SpriteVS.hlsl", vsModel);
	shaderObjectPS["SPRITE"] = CompileShader(L"SpritePS.hlsl", psModel);
	//DrawLine2dシェーダー
	shaderObjectVS["DRAW_LINE_2D"] = CompileShader(L"DrawLine2DVS.hlsl", vsModel);
	shaderObjectPS["DRAW_LINE_2D"] = CompileShader(L"DrawLine2DPS.hlsl", psModel);
	//PostEffectシェーダー
	shaderObjectVS["POST_EFFECT"] = CompileShader(L"PostEffectVS.hlsl", vsModel);
	shaderObjectPS["POST_EFFECT"] = CompileShader(L"PostEffectPS.hlsl", psModel);
	//Particleシェーダー
	shaderObjectVS["PARTICLE"] = CompileShader(L"ParticleVS.hlsl", vsModel);
	shaderObjectPS["PARTICLE"] = CompileShader(L"ParticlePS.hlsl", psModel);
	shaderObjectGS["PARTICLE"] = CompileShader(L"ParticleGS.hlsl", gsModel);
	//CubeBoxシェーダー
	shaderObjectVS["CUBE_BOX"] = CompileShader(L"CubeBoxVS.hlsl", vsModel);
	shaderObjectPS["CUBE_BOX"] = CompileShader(L"CubeBoxPS.hlsl", psModel);

}

void ShaderManager::Finalize()
{
	for (auto shader = shaderObjectVS.begin(); shader != shaderObjectVS.end(); shader++)
	{
		shader->second.Reset();
	}
	shaderObjectVS.clear();

	for (auto shader = shaderObjectPS.begin(); shader != shaderObjectPS.end(); shader++)
	{
		shader->second.Reset();
	}
	shaderObjectPS.clear();

	for (auto shader = shaderObjectGS.begin(); shader != shaderObjectGS.end(); shader++)
	{
		shader->second.Reset();
	}
	shaderObjectGS.clear();

	for (auto shader = shaderObjectCS.begin(); shader != shaderObjectCS.end(); shader++)
	{
		shader->second.Reset();
	}
	shaderObjectCS.clear();

}