#include "ShaderManager.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

const std::wstring pass = L"Resources/Shaders/";

//�G���[�̏o��
void OutputErrstr(ID3DBlob* errorBlob)
{
	// errorBlob����G���[���e��string�^�ɃR�s�[
	std::string errstr;
	errstr.resize(errorBlob->GetBufferSize());

	std::copy_n((char*)errorBlob->GetBufferPointer(),
		errorBlob->GetBufferSize(),
		errstr.begin());
	errstr += "\n";
	// �G���[���e���o�̓E�B���h�E�ɕ\��
	OutputDebugStringA(errstr.c_str());
	exit(1);
}

//�p�X�̎擾
std::wstring GetFullPath(LPCWSTR assetName)
{
	return pass + assetName;
}

//�V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
ID3DBlob* CompileShader(const LPCWSTR fileName, LPCSTR target)
{
	HRESULT result = S_FALSE;
	//�o�͗p�I�u�W�F�N�g
	ID3DBlob* blob = nullptr;
	//�C���N���[�h�ݒ�
	ID3DInclude* includeFrag = D3D_COMPILE_STANDARD_FILE_INCLUDE;
	// �f�o�b�O�p�ݒ�
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	//�G���[�I�u�W�F�N�g
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
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	ShaderManager* instance = new ShaderManager();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	instance->LoadShader();

	return std::unique_ptr<ShaderManager>(instance);
}

void ShaderManager::LoadShader()
{
	HRESULT result = S_FALSE;

	//���_�V�F�[�_�[���f��
	LPCSTR vsModel = "vs_5_0";
	//�s�N�Z���V�F�[�_�[���f��
	LPCSTR psModel = "ps_5_0";
	//�W�I���g���V�F�[�_�[���f��
	LPCSTR gsModel = "gs_5_0";
	//�R���s���[�g�V�F�[�_�[���f��
	LPCSTR csModel = "cs_5_0";

	//Obj�V�F�[�_�[
	obj= std::make_unique<BLOB_OBJECT>();
	obj->vs = CompileShader(L"ObjVS.hlsl", vsModel);
	obj->ps = CompileShader(L"ObjPS.hlsl", psModel);

	//Fbx�V�F�[�_�[
	fbx = std::make_unique<BLOB_OBJECT>();
	fbx->vs = CompileShader(L"FbxVS.hlsl", vsModel);
	fbx->ps = CompileShader(L"FbxPS.hlsl", psModel);

	//DrawLine3d�V�F�[�_�[
	drawLine3d = std::make_unique<BLOB_OBJECT>();
	drawLine3d->vs = CompileShader(L"DrawLine3DVS.hlsl", vsModel);
	drawLine3d->ps = CompileShader(L"DrawLine3DPS.hlsl", psModel);

	//NormalMap�V�F�[�_�[
	normalMap = std::make_unique<BLOB_OBJECT>();
	normalMap->vs = CompileShader(L"NormalMapVS.hlsl", vsModel);
	normalMap->ps = CompileShader(L"NormalMapPS.hlsl", psModel);

	//Sprite�V�F�[�_�[
	sprite = std::make_unique<BLOB_OBJECT>();
	sprite->vs = CompileShader(L"SpriteVS.hlsl", vsModel);
	sprite->ps = CompileShader(L"SpritePS.hlsl", psModel);

	//DrawLine2d�V�F�[�_�[
	drawLine2d = std::make_unique<BLOB_OBJECT>();
	drawLine2d->vs = CompileShader(L"DrawLine2DVS.hlsl", vsModel);
	drawLine2d->ps = CompileShader(L"DrawLine2DPS.hlsl", psModel);

	//PostEffect�V�F�[�_�[
	postEffect = std::make_unique<BLOB_OBJECT>();
	postEffect->vs = CompileShader(L"PostEffectVS.hlsl", vsModel);
	postEffect->ps = CompileShader(L"PostEffectPS.hlsl", psModel);

	//Particle�V�F�[�_�[
	particle = std::make_unique<BLOB_OBJECT>();
	particle->vs = CompileShader(L"ParticleVS.hlsl", vsModel);
	particle->ps = CompileShader(L"ParticlePS.hlsl", psModel);
	particle->gs = CompileShader(L"ParticleGS.hlsl", gsModel);

	//co�V�F�[�_�[
	compute = std::make_unique<BLOB_OBJECT>();
	compute->cs = CompileShader(L"BasicCS.hlsl", csModel);

	//Pmx�V�F�[�_�[
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