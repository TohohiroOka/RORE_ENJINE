#include "Fog.h"

void Fog::CreateConstBuffer()
{
	// 定数バッファの生成
	HRESULT result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(CONST_BUFFER_DATA) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)) { assert(0); }

}

void Fog::Update()
{
	// 定数バッファへデータ転送
	CONST_BUFFER_DATA* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->strength = strength;
		constMap->color = color;
		constBuff->Unmap(0, nullptr);
	}
}

Fog::Fog()
{
	//フォグの強さ
	strength = 1.5f;
	color = { 1.0f,1.0f,1.0f };
}

std::unique_ptr<Fog> Fog::Create()
{
	// Spriteのインスタンスを生成
	Fog* instance = new Fog();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize(EffectTyep::fog);
	//定数バッファ
	instance->CreateConstBuffer();
	//深度バッファ生成
	instance->CreateDepthBuffer();

	//ユニークポインタを返す
	return std::unique_ptr<Fog>(instance);
}

void Fog::Draw(const Texture*_tex)
{
	Fog::Update();

	BasePostEffect::Draw(_tex);
}