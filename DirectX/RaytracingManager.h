#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class Camera;

//シェーダー系の変数クラス
class HlslCompat
{
public:

	struct ProceduralPrimitiveAttributes
	{
		DirectX::XMFLOAT3 normal;
	};

	struct RayPayload
	{
		DirectX::XMFLOAT4 color;
		UINT   recursionDepth;
	};

	struct ShadowRayPayload
	{
		bool hit;
	};

	// Attributes per primitive type.
	struct PrimitiveConstantBuffer
	{
		DirectX::XMFLOAT4 albedo;
		float reflectanceCoef;
		float diffuseCoef;
		float specularCoef;
		float specularPower;
		float stepScale;                      // Step scale for ray marching of signed distance primitives. 
											  // - Some object transformations don't preserve the distances and 
											  //   thus require shorter steps.
		DirectX::XMFLOAT3 pad;
	};

	// Attributes per primitive instance.
	struct PrimitiveInstanceConstantBuffer
	{
		UINT instanceIndex;
		UINT primitiveType; // Procedural primitive type
	};

	// Ray types traced in this sample.
	enum class RayType {
		Radiance = 0,   // ~ Primary, reflected camera/view rays calculating color for each hit.
		Shadow,         // ~ Shadow/visibility rays, only testing for occlusion
		Count
	};

	enum class AnalyticPrimitive {
		AABB = 0,
		Spheres,
		Count
	};

	enum class VolumetricPrimitive {
		Metaballs = 0,
		Count
	};

	enum class SignedDistancePrimitive {
		MiniSpheres = 0,
		IntersectedRoundCube,
		SquareTorus,
		TwistedTorus,
		Cog,
		Cylinder,
		FractalPyramid,
		Count
	};

public:

	static const UINT MAX_RAY_RECURSION_DEPTH = 3;

};

//パイプライン系の設定
class SceneDefines
{
public:

	//グローバルルートシグネチャのスロット
	enum class Global_RootSignature_Slot {
		OutputView = 0,
		AccelerationStructure,
		SceneConstant,
		AABBattributeBuffer,
		VertexBuffers,
		Count
	};

	//ローカルルートシグネチャの種類
	enum class Local_RootSignature_Type {
		Triangle = 0,
		AABB,
		Count
	};

	enum class Local_RootSignature_Triangle_Slot {
		MaterialConstant = 0,
		Count
	};

	enum class Local_RootSignature_AABB_Slot {
		MaterialConstant = 0,
		GeometryIndex,
		Count
	};

	enum class GeometryType {
		Triangle = 0,
		AABB,       // Procedural geometry with an application provided AABB.
		Count
	};

	enum class IntersectionShaderType {
		AnalyticPrimitive = 0,
		VolumetricPrimitive,
		SignedDistancePrimitive,
		Count
	};

	inline UINT PerPrimitiveTypeCount(IntersectionShaderType type)
	{
		switch (type)
		{
		case IntersectionShaderType::AnalyticPrimitive: return (int)HlslCompat::AnalyticPrimitive::Count;
		case IntersectionShaderType::VolumetricPrimitive: return (int)HlslCompat::VolumetricPrimitive::Count;
		case IntersectionShaderType::SignedDistancePrimitive: return (int)HlslCompat::SignedDistancePrimitive::Count;
		}
		return 0;
	}
};

class RaytracingManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;


private://構造体宣言

public://静的メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="cmdList">コマンドリスト</param>
	static void StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<RaytracingManager> Create();

	/// <summary>
	/// 解放処理
	/// </summary>
	static void Finalize();

private://静的メンバ関数

	/// <summary>
	/// レイトレーシングデバイスとコマンドリストの作成
	/// </summary>
	static void CreateRaytracingInterfaces();

	/// <summary>
	/// レイトレーシング用ルートシグネチャの生成
	/// </summary>
	/// <param name="desc">シグネチャ設定</param>
	/// <param name="rootSig"></param>
	static void SerializeAndCreateRaytracingRootSignature(
		D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootSig);

	/// <summary>
	/// ルートシグネチャの生成
	/// </summary>
	static void CreateRootSignatures();

	/// <summary>
	/// ???????????????????????
	/// </summary>
	/// <param name="raytracingPipeline">パイプライン</param>
	static void CreateDxilLibrarySubobject(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);

	/// <summary>
	/// 光線がジオメトリと交差するときに実行される最も近いヒットおよび交差シェーダーを指定
	/// </summary>
	/// <param name="raytracingPipeline">パイプライン</param>
	static void CreateHitGroupSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);

	/// <summary>
	/// ローカルルート署名とシェーダーの関連付け
	/// </summary>
	/// <param name="raytracingPipeline">パイプライン</param>
	static void CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="desc"></param>
	static void PrintStateObjectDesc(const D3D12_STATE_OBJECT_DESC* desc);

	/// <summary>
	/// レイトレーシングパイプライン状態オブジェクト（RTPSO）の作成
	/// </summary>
	static void CreateRaytracingPipelineStateObject();

	/// <summary>
	/// デスクリプタヒープ生成
	/// </summary>
	static void CreateDescriptorHeap();

private://メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

public://メンバ関数


private://変数

	// デバイス
	static ID3D12Device* device;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;

	// DirectX Raytracing (DXR) attributes
	static ComPtr<ID3D12Device5> m_dxrDevice;
	static ComPtr<ID3D12GraphicsCommandList5> m_dxrCommandList;
	static ComPtr<ID3D12StateObject> m_dxrStateObject;

	// Root signatures
	static ComPtr<ID3D12RootSignature> m_raytracingGlobalRootSignature;
	static ComPtr<ID3D12RootSignature> m_raytracingLocalRootSignature[(int)SceneDefines::Local_RootSignature_Type::Count];

	// Descriptors
	static ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
	static UINT m_descriptorsAllocated;
	static UINT m_descriptorSize;


	// Shader tables
	static const wchar_t* c_hitGroupNames_TriangleGeometry[(int)HlslCompat::RayType::Count];
	static const wchar_t* c_hitGroupNames_AABBGeometry[(int)SceneDefines::IntersectionShaderType::Count][(int)HlslCompat::RayType::Count];
	static const wchar_t* c_raygenShaderName;
	static const wchar_t* c_intersectionShaderNames[(int)SceneDefines::IntersectionShaderType::Count];
	static const wchar_t* c_closestHitShaderNames[(int)SceneDefines::GeometryType::Count];
	static const wchar_t* c_missShaderNames[(int)HlslCompat::RayType::Count];

};