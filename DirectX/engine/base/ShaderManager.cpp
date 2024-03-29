#include "ShaderManager.h"
#include "Helpar.h"
#include <JsonLoader.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

const std::wstring pass = L"Resources/Shaders/";

//エラーの出力
void OutputErrstr(ID3DBlob* _errorBlob)
{
	// errorBlobからエラー内容をstring型にコピー
	std::string errstr;
	errstr.resize(_errorBlob->GetBufferSize());

	std::copy_n((char*)_errorBlob->GetBufferPointer(),
		_errorBlob->GetBufferSize(),
		errstr.begin());
	errstr += "\n";
	// エラー内容を出力ウィンドウに表示
	OutputDebugStringA(errstr.c_str());
	exit(1);
}

//パスの取得
std::wstring GetFullPath(LPCWSTR _assetName)
{
	return pass + _assetName;
}

//シェーダーの読み込みとコンパイル
ID3DBlob* CompileShader(const LPCWSTR _fileName, LPCSTR _target)
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

	result = D3DCompileFromFile(GetFullPath(_fileName).c_str(), nullptr, includeFrag, "main", _target,
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

	std::vector<JsonLoader::Shader> shader{};
	JsonLoader::LoadShaderName("Shader", &shader);
	for (auto& i : shader) {
		for (auto& j : i.type) {
			std::string inName;
			if (j == "v") {
				inName = i.name + "VS";
				shaderObject[inName] = CompileShader(GetName(inName, ".hlsl"), vsModel);
			} else if (j == "p") {
				inName = i.name + "PS";
				shaderObject[inName] = CompileShader(GetName(inName, ".hlsl"), psModel);
			} else if (j == "g") {
				inName = i.name + "GS";
				shaderObject[inName] = CompileShader(GetName(inName, ".hlsl"), gsModel);
			}
		}
	}
}

void ShaderManager::Finalize()
{
	for (auto shader : shaderObject)
	{
		shader.second.Reset();
	}
	shaderObject.clear();

	for (auto shader : shaderObjectCS)
	{
		shader.second.Reset();
	}
	shaderObjectCS.clear();
}