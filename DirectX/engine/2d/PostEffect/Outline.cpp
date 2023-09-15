#include "Outline.h"

void Outline::CreateConstBuffer()
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

void Outline::Update()
{
	// 定数バッファへデータ転送
	CONST_BUFFER_DATA* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->outlineWidth = outlineWidth;
		constBuff->Unmap(0, nullptr);
	}
}

Outline::Outline()
	:BasePostEffect()
{
	//アウトラインの太さ初期化
	outlineWidth = 1.0f;
}

std::unique_ptr<Outline> Outline::Create()
{
	// Spriteのインスタンスを生成
	Outline* instance = new Outline();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize(EffectTyep::outline);
	//定数バッファ
	instance->CreateConstBuffer();
	//深度バッファ生成
	instance->CreateDepthBuffer();

	//ユニークポインタを返す
	return std::unique_ptr<Outline>(instance);
}

void Outline::Draw(const Texture* _tex)
{
	Update();

	BasePostEffect::Draw(_tex);
}