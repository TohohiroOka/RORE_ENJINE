#include "DescriptorHeapManager.h"

ID3D12Device* DescriptorHeapManager::device = nullptr;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHeapManager::descHeap = nullptr;

void DescriptorHeapManager::StaticInitialize(ID3D12Device* device)
{
	HRESULT result = S_FALSE;

	// nullptrチェック
	assert(!DescriptorHeapManager::device);
	assert(device);
	DescriptorHeapManager::device = device;

	//デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 512;
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));
}

void DescriptorHeapManager::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	//デスクリプタヒープをセット
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void DescriptorHeapManager::CreateSRV(
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuffer, D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc)
{
	static int heapNumber = 0;

	cpu = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), heapNumber,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	gpu = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap->GetGPUDescriptorHandleForHeapStart(), heapNumber,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	//ヒープのtexnumber番目にシェーダーリソースビューを作成
	device->CreateShaderResourceView(
		texBuffer.Get(),
		&srvDesc,
		cpu);

	heapNumber++;
}