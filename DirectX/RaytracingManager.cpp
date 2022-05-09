#include "RaytracingManager.h"
#include "Raytracing.hlsl.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace DirectX;
using namespace Microsoft::WRL;

ID3D12Device* RaytracingManager::device = nullptr;
ID3D12GraphicsCommandList* RaytracingManager::cmdList = nullptr;
ComPtr<ID3D12Device5> RaytracingManager::m_dxrDevice = nullptr;
ComPtr<ID3D12GraphicsCommandList5> RaytracingManager::m_dxrCommandList = nullptr;
ComPtr<ID3D12StateObject> RaytracingManager::m_dxrStateObject = nullptr;
ComPtr<ID3D12RootSignature> RaytracingManager::m_raytracingGlobalRootSignature = nullptr;
ComPtr<ID3D12RootSignature> RaytracingManager::m_raytracingLocalRootSignature[(int)SceneDefines::Local_RootSignature_Type::Count] = { nullptr };
ComPtr<ID3D12DescriptorHeap> RaytracingManager::m_descriptorHeap = nullptr;
UINT RaytracingManager::m_descriptorsAllocated = 0;
UINT RaytracingManager::m_descriptorSize = 0;

// Shader entry points.
const wchar_t* RaytracingManager::c_raygenShaderName = L"MyRaygenShader";
const wchar_t* RaytracingManager::c_intersectionShaderNames[] =
{
	L"MyIntersectionShader_AnalyticPrimitive",
	L"MyIntersectionShader_VolumetricPrimitive",
	L"MyIntersectionShader_SignedDistancePrimitive",
};
const wchar_t* RaytracingManager::c_closestHitShaderNames[] =
{
	L"MyClosestHitShader_Triangle",
	L"MyClosestHitShader_AABB",
};
const wchar_t* RaytracingManager::c_missShaderNames[] =
{
	L"MyMissShader", L"MyMissShader_ShadowRay"
};
// Hit groups.
const wchar_t* RaytracingManager::c_hitGroupNames_TriangleGeometry[] =
{
	L"MyHitGroup_Triangle", L"MyHitGroup_Triangle_ShadowRay"
};
const wchar_t* RaytracingManager::c_hitGroupNames_AABBGeometry[][(int)HlslCompat::RayType::Count] =
{
	{ L"MyHitGroup_AABB_AnalyticPrimitive", L"MyHitGroup_AABB_AnalyticPrimitive_ShadowRay" },
	{ L"MyHitGroup_AABB_VolumetricPrimitive", L"MyHitGroup_AABB_VolumetricPrimitive_ShadowRay" },
	{ L"MyHitGroup_AABB_SignedDistancePrimitive", L"MyHitGroup_AABB_SignedDistancePrimitive_ShadowRay" },
};

void RaytracingManager::StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	// nullptrチェック
	assert(!RaytracingManager::device);
	assert(device);

	RaytracingManager::device = device;

	// nullptrチェック
	assert(!RaytracingManager::cmdList);
	assert(cmdList);

	RaytracingManager::cmdList = cmdList;

	CreateRaytracingInterfaces();

	CreateRootSignatures();

	CreateRaytracingPipelineStateObject();

	CreateDescriptorHeap();
}

std::unique_ptr<RaytracingManager> RaytracingManager::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	RaytracingManager* instance = new RaytracingManager();

	//初期化
	instance->Initialize();

	return std::unique_ptr<RaytracingManager>();
}

void RaytracingManager::Finalize()
{
	m_dxrDevice.Reset();
	m_dxrCommandList.Reset();
	m_dxrStateObject.Reset();
	m_raytracingGlobalRootSignature.Reset();
	for (int i = 0; i < (int)SceneDefines::Local_RootSignature_Type::Count; i++)
	{
		m_raytracingLocalRootSignature[i].Reset();
	}
	m_descriptorHeap.Reset();
}

void RaytracingManager::CreateRaytracingInterfaces()
{
	HRESULT result;

	result = device->QueryInterface(IID_PPV_ARGS(&m_dxrDevice));
	assert(SUCCEEDED(result));

	result = cmdList->QueryInterface(IID_PPV_ARGS(&m_dxrCommandList));
	assert(SUCCEEDED(result));
}

void RaytracingManager::SerializeAndCreateRaytracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootSig)
{
	HRESULT result;
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;

	result = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	assert(SUCCEEDED(result));

	result = device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(*rootSig)));
	assert(SUCCEEDED(result));
}

void RaytracingManager::CreateRootSignatures()
{
	// Global Root Signature
	// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
	{
		CD3DX12_DESCRIPTOR_RANGE ranges[2]; // Perfomance TIP: Order from most frequent to least frequent.
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);  // 1 output texture
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);  // 2 static index and vertex buffers.

		CD3DX12_ROOT_PARAMETER rootParameters[(int)SceneDefines::Global_RootSignature_Slot::Count];
		rootParameters[(int)SceneDefines::Global_RootSignature_Slot::OutputView].InitAsDescriptorTable(1, &ranges[0]);
		rootParameters[(int)SceneDefines::Global_RootSignature_Slot::AccelerationStructure].InitAsShaderResourceView(0);
		rootParameters[(int)SceneDefines::Global_RootSignature_Slot::SceneConstant].InitAsConstantBufferView(0);
		rootParameters[(int)SceneDefines::Global_RootSignature_Slot::AABBattributeBuffer].InitAsShaderResourceView(3);
		rootParameters[(int)SceneDefines::Global_RootSignature_Slot::VertexBuffers].InitAsDescriptorTable(1, &ranges[1]);
		CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		SerializeAndCreateRaytracingRootSignature(globalRootSignatureDesc, &m_raytracingGlobalRootSignature);
	}

	// Local Root Signature
	// This is a root signature that enables a shader to have unique arguments that come from shader tables.
	{
		UINT ConstantBufferSize = (sizeof(HlslCompat::PrimitiveConstantBuffer) - 1) / sizeof(UINT32) + 1;

		// Triangle geometry
		{
			CD3DX12_ROOT_PARAMETER rootParameters[(int)SceneDefines::Local_RootSignature_Triangle_Slot::Count];
			rootParameters[(int)SceneDefines::Local_RootSignature_Triangle_Slot::MaterialConstant].InitAsConstants(ConstantBufferSize, 1);
			
			CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
			localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
			SerializeAndCreateRaytracingRootSignature(localRootSignatureDesc, &m_raytracingLocalRootSignature[(int)SceneDefines::Local_RootSignature_Type::Triangle]);
		}

		// AABB geometry
		{
			CD3DX12_ROOT_PARAMETER rootParameters[(int)SceneDefines::Local_RootSignature_AABB_Slot::Count];
			rootParameters[(int)SceneDefines::Local_RootSignature_AABB_Slot::MaterialConstant].InitAsConstants(ConstantBufferSize, 1);
			UINT InstanceConstantBufferSize = (sizeof(HlslCompat::PrimitiveInstanceConstantBuffer) - 1) / sizeof(UINT32) + 1;
			rootParameters[(int)SceneDefines::Local_RootSignature_AABB_Slot::GeometryIndex].InitAsConstants(InstanceConstantBufferSize, 2);

			CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
			localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
			SerializeAndCreateRaytracingRootSignature(localRootSignatureDesc, &m_raytracingLocalRootSignature[(int)SceneDefines::Local_RootSignature_Type::AABB]);
		}
	}
}

void RaytracingManager::CreateDxilLibrarySubobject(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
{
	auto lib = raytracingPipeline->CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
	D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((void*)g_pRaytracing, ARRAYSIZE(g_pRaytracing));
	lib->SetDXILLibrary(&libdxil);
	// Use default shader exports for a DXIL library/collection subobject ~ surface all shaders.
}

void RaytracingManager::CreateHitGroupSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
{
		HRESULT result;

	// Triangle geometry hit groups
	{
		for (UINT rayType = 0; rayType < (int)HlslCompat::RayType::Count; rayType++)
		{
			auto hitGroup = raytracingPipeline->CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
			if (rayType == (int)HlslCompat::RayType::Radiance)
			{
				hitGroup->SetClosestHitShaderImport(c_closestHitShaderNames[(int)SceneDefines::GeometryType::Triangle]);
			}
			hitGroup->SetHitGroupExport(c_hitGroupNames_TriangleGeometry[rayType]);
			hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);
		}
	}

	// AABB geometry hit groups
	{
		// Create hit groups for each intersection shader.
		for (UINT t = 0; t < (int)SceneDefines::IntersectionShaderType::Count; t++)
			for (UINT rayType = 0; rayType < (int)HlslCompat::RayType::Count; rayType++)
			{
				auto hitGroup = raytracingPipeline->CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
				hitGroup->SetIntersectionShaderImport(c_intersectionShaderNames[t]);
				if (rayType == (int)HlslCompat::RayType::Radiance)
				{
					hitGroup->SetClosestHitShaderImport(c_closestHitShaderNames[(int)SceneDefines::GeometryType::AABB]);
				}
				hitGroup->SetHitGroupExport(c_hitGroupNames_AABBGeometry[t][rayType]);
				hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE);
			}
	}
}

void RaytracingManager::CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
{
	// Ray gen and miss shaders in this sample are not using a local root signature and thus one is not associated with them.

	// Hit groups
	// Triangle geometry
	{
		auto localRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		localRootSignature->SetRootSignature(m_raytracingLocalRootSignature[(int)SceneDefines::Local_RootSignature_Type::Triangle].Get());
		// Shader association
		auto rootSignatureAssociation = raytracingPipeline->CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
		rootSignatureAssociation->AddExports(c_hitGroupNames_TriangleGeometry);
	}

	// AABB geometry
	{
		auto localRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		localRootSignature->SetRootSignature(m_raytracingLocalRootSignature[(int)SceneDefines::Local_RootSignature_Type::AABB].Get());
		// Shader association
		auto rootSignatureAssociation = raytracingPipeline->CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
		for (auto& hitGroupsForIntersectionShaderType : c_hitGroupNames_AABBGeometry)
		{
			rootSignatureAssociation->AddExports(hitGroupsForIntersectionShaderType);
		}
	}
}

// Pretty-print a state object tree.
void RaytracingManager::PrintStateObjectDesc(const D3D12_STATE_OBJECT_DESC* desc)
{
	std::wstringstream wstr;
	wstr << L"\n";
	wstr << L"--------------------------------------------------------------------\n";
	wstr << L"| D3D12 State Object 0x" << static_cast<const void*>(desc) << L": ";
	if (desc->Type == D3D12_STATE_OBJECT_TYPE_COLLECTION) wstr << L"Collection\n";
	if (desc->Type == D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE) wstr << L"Raytracing Pipeline\n";

	auto ExportTree = [](UINT depth, UINT numExports, const D3D12_EXPORT_DESC* exports)
	{
		std::wostringstream woss;
		for (UINT i = 0; i < numExports; i++)
		{
			woss << L"|";
			if (depth > 0)
			{
				for (UINT j = 0; j < 2 * depth - 1; j++) woss << L" ";
			}
			woss << L" [" << i << L"]: ";
			if (exports[i].ExportToRename) woss << exports[i].ExportToRename << L" --> ";
			woss << exports[i].Name << L"\n";
		}
		return woss.str();
	};

	for (UINT i = 0; i < desc->NumSubobjects; i++)
	{
		wstr << L"| [" << i << L"]: ";
		switch (desc->pSubobjects[i].Type)
		{
		case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
			wstr << L"Global Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
			break;
		case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
			wstr << L"Local Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
			break;
		case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK:
			wstr << L"Node Mask: 0x" << std::hex << std::setfill(L'0') << std::setw(8) << *static_cast<const UINT*>(desc->pSubobjects[i].pDesc) << std::setw(0) << std::dec << L"\n";
			break;
		case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
		{
			wstr << L"DXIL Library 0x";
			auto lib = static_cast<const D3D12_DXIL_LIBRARY_DESC*>(desc->pSubobjects[i].pDesc);
			wstr << lib->DXILLibrary.pShaderBytecode << L", " << lib->DXILLibrary.BytecodeLength << L" bytes\n";
			wstr << ExportTree(1, lib->NumExports, lib->pExports);
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
		{
			wstr << L"Existing Library 0x";
			auto collection = static_cast<const D3D12_EXISTING_COLLECTION_DESC*>(desc->pSubobjects[i].pDesc);
			wstr << collection->pExistingCollection << L"\n";
			wstr << ExportTree(1, collection->NumExports, collection->pExports);
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
		{
			wstr << L"Subobject to Exports Association (Subobject [";
			auto association = static_cast<const D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
			UINT index = static_cast<UINT>(association->pSubobjectToAssociate - desc->pSubobjects);
			wstr << index << L"])\n";
			for (UINT j = 0; j < association->NumExports; j++)
			{
				wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
			}
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
		{
			wstr << L"DXIL Subobjects to Exports Association (";
			auto association = static_cast<const D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
			wstr << association->SubobjectToAssociate << L")\n";
			for (UINT j = 0; j < association->NumExports; j++)
			{
				wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
			}
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG:
		{
			wstr << L"Raytracing Shader Config\n";
			auto config = static_cast<const D3D12_RAYTRACING_SHADER_CONFIG*>(desc->pSubobjects[i].pDesc);
			wstr << L"|  [0]: Max Payload Size: " << config->MaxPayloadSizeInBytes << L" bytes\n";
			wstr << L"|  [1]: Max Attribute Size: " << config->MaxAttributeSizeInBytes << L" bytes\n";
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG:
		{
			wstr << L"Raytracing Pipeline Config\n";
			auto config = static_cast<const D3D12_RAYTRACING_PIPELINE_CONFIG*>(desc->pSubobjects[i].pDesc);
			wstr << L"|  [0]: Max Recursion Depth: " << config->MaxTraceRecursionDepth << L"\n";
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP:
		{
			wstr << L"Hit Group (";
			auto hitGroup = static_cast<const D3D12_HIT_GROUP_DESC*>(desc->pSubobjects[i].pDesc);
			wstr << (hitGroup->HitGroupExport ? hitGroup->HitGroupExport : L"[none]") << L")\n";
			wstr << L"|  [0]: Any Hit Import: " << (hitGroup->AnyHitShaderImport ? hitGroup->AnyHitShaderImport : L"[none]") << L"\n";
			wstr << L"|  [1]: Closest Hit Import: " << (hitGroup->ClosestHitShaderImport ? hitGroup->ClosestHitShaderImport : L"[none]") << L"\n";
			wstr << L"|  [2]: Intersection Import: " << (hitGroup->IntersectionShaderImport ? hitGroup->IntersectionShaderImport : L"[none]") << L"\n";
			break;
		}
		}
		wstr << L"|--------------------------------------------------------------------\n";
	}
	wstr << L"\n";
	OutputDebugStringW(wstr.str().c_str());
}

void RaytracingManager::CreateRaytracingPipelineStateObject()
{
	HRESULT result;

	// Create 18 subobjects that combine into a RTPSO:
	// Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations.
	// Default association applies to every exported shader entrypoint that doesn't have any of the same type of subobject associated with it.
	// This simple sample utilizes default shader association except for local root signature subobject
	// which has an explicit association specified purely for demonstration purposes.
	// 1 - DXIL library
	// 8 - Hit group types - 4 geometries (1 triangle, 3 aabb) x 2 ray types (ray, shadowRay)
	// 1 - Shader config
	// 6 - 3 x Local root signature and association
	// 1 - Global root signature
	// 1 - Pipeline config
	CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

	// DXIL library
	CreateDxilLibrarySubobject(&raytracingPipeline);

	// Hit groups
	CreateHitGroupSubobjects(&raytracingPipeline);

	// Shader config
	// Defines the maximum sizes in bytes for the ray rayPayload and attribute structure.
	auto shaderConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	UINT payloadSize = 0;
	if (sizeof(HlslCompat::RayPayload) > sizeof(HlslCompat::ShadowRayPayload))
	{
		payloadSize = sizeof(HlslCompat::ShadowRayPayload);
	}
	else { payloadSize = sizeof(HlslCompat::RayPayload); }
	UINT attributeSize = sizeof(struct HlslCompat::ProceduralPrimitiveAttributes);
	shaderConfig->Config(payloadSize, attributeSize);

	// Local root signature and shader association
	// This is a root signature that enables a shader to have unique arguments that come from shader tables.
	CreateLocalRootSignatureSubobjects(&raytracingPipeline);

	// Global root signature
	// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
	auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	globalRootSignature->SetRootSignature(m_raytracingGlobalRootSignature.Get());

	// Pipeline config
	// Defines the maximum TraceRay() recursion depth.
	auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	// PERFOMANCE TIP: Set max recursion depth as low as needed
	// as drivers may apply optimization strategies for low recursion depths.
	UINT maxRecursionDepth = HlslCompat::MAX_RAY_RECURSION_DEPTH;
	pipelineConfig->Config(maxRecursionDepth);

	PrintStateObjectDesc(raytracingPipeline);

	// Create the state object.
	result = m_dxrDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&m_dxrStateObject));
	assert(SUCCEEDED(result));
}

void RaytracingManager::CreateDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	// Allocate a heap for 6 descriptors:
	// 2 - vertex and index  buffer SRVs
	// 1 - raytracing output texture SRV
	descriptorHeapDesc.NumDescriptors = 3;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));
	m_descriptorHeap->SetName(L"RaytracingDescriptor");

	m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void RaytracingManager::Initialize()
{

}
