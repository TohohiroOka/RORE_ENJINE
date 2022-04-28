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
	obj= std::make_unique<BLOB_OBJECT>();
	obj->vs = CompileShader(L"ObjVS.hlsl", vsModel);
	obj->ps = CompileShader(L"ObjPS.hlsl", psModel);

	//Fbxシェーダー
	fbx = std::make_unique<BLOB_OBJECT>();
	fbx->vs = CompileShader(L"FbxVS.hlsl", vsModel);
	fbx->ps = CompileShader(L"FbxPS.hlsl", psModel);

	//DrawLine3dシェーダー
	drawLine3d = std::make_unique<BLOB_OBJECT>();
	drawLine3d->vs = CompileShader(L"DrawLine3DVS.hlsl", vsModel);
	drawLine3d->ps = CompileShader(L"DrawLine3DPS.hlsl", psModel);

	//NormalMapシェーダー
	normalMap = std::make_unique<BLOB_OBJECT>();
	normalMap->vs = CompileShader(L"NormalMapVS.hlsl", vsModel);
	normalMap->ps = CompileShader(L"NormalMapPS.hlsl", psModel);

	//Spriteシェーダー
	sprite = std::make_unique<BLOB_OBJECT>();
	sprite->vs = CompileShader(L"SpriteVS.hlsl", vsModel);
	sprite->ps = CompileShader(L"SpritePS.hlsl", psModel);

	//DrawLine2dシェーダー
	drawLine2d = std::make_unique<BLOB_OBJECT>();
	drawLine2d->vs = CompileShader(L"DrawLine2DVS.hlsl", vsModel);
	drawLine2d->ps = CompileShader(L"DrawLine2DPS.hlsl", psModel);

	//PostEffectシェーダー
	postEffect = std::make_unique<BLOB_OBJECT>();
	postEffect->vs = CompileShader(L"PostEffectVS.hlsl", vsModel);
	postEffect->ps = CompileShader(L"PostEffectPS.hlsl", psModel);

	//Particleシェーダー
	particle = std::make_unique<BLOB_OBJECT>();
	particle->vs = CompileShader(L"ParticleVS.hlsl", vsModel);
	particle->ps = CompileShader(L"ParticlePS.hlsl", psModel);
	particle->gs = CompileShader(L"ParticleGS.hlsl", gsModel);

	//coシェーダー
	compute = std::make_unique<BLOB_OBJECT>();
	compute->cs = CompileShader(L"BasicCS.hlsl", csModel);

	//Pmxシェーダー
	pmx = std::make_unique<BLOB_OBJECT>();
	pmx->vs = CompileShader(L"PmxVS.hlsl", vsModel);
	pmx->ps = CompileShader(L"PmxPS.hlsl", psModel);
}

void ShaderManager::Finalize()
{
	obj.reset();
	sprite.reset();
	fbx.reset();
	drawLine2d.reset();
	drawLine3d.reset();
	postEffect.reset();
	normalMap.reset();
	particle.reset();
	pmx.reset();
}