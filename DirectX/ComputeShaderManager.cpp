#include "ComputeShaderManager.h"

#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

ID3D12Device* ComputeShaderManager::device = nullptr;
ID3D12GraphicsCommandList* ComputeShaderManager::cmdList = nullptr;
ComPtr<ID3DBlob> ComputeShaderManager::computeShader = nullptr;
ComPtr<ID3D12RootSignature> ComputeShaderManager::rootSignature = nullptr;
ComPtr<ID3D12PipelineState> ComputeShaderManager::pipelineState = nullptr;
ComPtr<ID3D12DescriptorHeap> ComputeShaderManager::Heap = nullptr;
//ComPtr<ID3D12DescriptorHeap> ComputeShaderManager::srvUavHeap = nullptr;

std::vector<float>test(256, 0);

ComputeShaderManager::~ComputeShaderManager()
{
	computeShader.Reset();
	rootSignature.Reset();
	pipelineState.Reset();
	Heap.Reset();
}

void ComputeShaderManager::StaticInitialize(ID3D12Device* device)
{
	ComputeShaderManager::device = device;
	//シェーダーの読み込み
	LoadShader();
	//パイプラインの生成
	CreatePipeline();
	//デスクリプタヒープの生成
	CreateDescriptor();
}

void ComputeShaderManager::Initialize()
{
	//定数バッファの生成
	CreateBuffers();
}

void ComputeShaderManager::LoadShader()
{
	HRESULT result = S_FALSE;

	//シェーダーコンパイル
	auto hr = D3DCompileFromFile(L"Resources/Shaders/BasicCS.hlsl", nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "cs_5_0", D3DCOMPILE_DEBUG |
		D3DCOMPILE_SKIP_OPTIMIZATION, 0, &computeShader, nullptr);
}

void ComputeShaderManager::CreatePipeline()
{
	HRESULT result = S_FALSE;

	//シェーダーからルートシグネチャ情報を取得
	ID3DBlob* sig = nullptr;
	result = D3DGetBlobPart(computeShader->GetBufferPointer(), computeShader->GetBufferSize(),
		D3D_BLOB_ROOT_SIGNATURE, 0, &sig);
	if (FAILED(result)) {
		assert(0);
	}

	//ルートシグネチャの生成
	result = device->CreateRootSignature(0, sig->GetBufferPointer(), sig->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	if (FAILED(result)) {
		assert(0);
	}

	//パイプラインの設定
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	desc.CS.pShaderBytecode = computeShader->GetBufferPointer();
	desc.CS.BytecodeLength = computeShader->GetBufferSize();
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.pRootSignature = rootSignature.Get();

	//設定の反映
	result = device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pipelineState));
	if (FAILED(result)) {
		assert(0);
	}

	rootSignature->SetName(L"cs_PEroot");
	pipelineState->SetName(L"cs_PEpipe");
}

void ComputeShaderManager::CreateDescriptor()
{
	HRESULT result = S_FALSE;

	//リソースは1つとして作成
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&Heap));
	if (FAILED(result)) {
		assert(0);
	}
}

void ComputeShaderManager::CreateBuffers()
{
	HRESULT result = S_FALSE;

	//定数バッファの設定
	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask = 1;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask = 1;
	//サイズは定数バッファと同じように指定
	D3D12_RESOURCE_DESC desc{};
	desc.Alignment = 0;
	desc.DepthOrArraySize = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Height = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.MipLevels = 1;
	desc.SampleDesc = { 1, 0 };
	desc.Width = (sizeof(float) * test.size() + 0xff) & ~0xff;

	//設定の反映
	result = device->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr,
		IID_PPV_ARGS(&inputBuffer));
	if (FAILED(result)) {
		assert(0);
	}

	//今回はGPUから受け取るのはchar型にしています
	D3D12_UNORDERED_ACCESS_VIEW_DESC outdesc{};
	outdesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	outdesc.Format = DXGI_FORMAT_UNKNOWN;
	outdesc.Buffer.NumElements = test.size();
	outdesc.Buffer.StructureByteStride = sizeof(float);

	device->CreateUnorderedAccessView(inputBuffer.Get(), nullptr, &outdesc, Heap->GetCPUDescriptorHandleForHeapStart());
}

XMFLOAT3* ComputeShaderManager::ShaderUpdate(UINT max, XMFLOAT3* startPosition, XMFLOAT3* endPosition,
	XMFLOAT3* nowPosition, float* time)
{
	HRESULT result;

	////定数バッファにデータを転送
	D3D12_RANGE range{ 0, 1 };
	result = inputBuffer->Map(0, &range, &data);
	inputBuffer->Unmap(0, &range);

	//パイプラインのセット
	cmdList->SetPipelineState(pipelineState.Get());

	//ルートシグネチャのセット
	cmdList->SetComputeRootSignature(rootSignature.Get());

	//デスクリプタヒープをセット
	ID3D12DescriptorHeap* ppHeaps[] = { Heap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	auto handle = Heap->GetGPUDescriptorHandleForHeapStart();
	cmdList->SetComputeRootDescriptorTable(0, handle);

	//コンピュートシェーダーの実行(今回は256個のスレッドグループを指定)
	cmdList->Dispatch(test.size(), 1, 1);

	//GPUからデータをもらう
	test.assign((float*)data, (float*)data + test.size());

	return 0;
}

void ComputeShaderManager::PreUpdate(ID3D12GraphicsCommandList* cmdList)
{
	ComputeShaderManager::cmdList = cmdList;
}

void ComputeShaderManager::PostUpdate()
{
	ComputeShaderManager::cmdList = nullptr;
}