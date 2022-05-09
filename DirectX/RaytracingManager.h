#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class Camera;

//�V�F�[�_�[�n�̕ϐ��N���X
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

//�p�C�v���C���n�̐ݒ�
class SceneDefines
{
public:

	//�O���[�o�����[�g�V�O�l�`���̃X���b�g
	enum class Global_RootSignature_Slot {
		OutputView = 0,
		AccelerationStructure,
		SceneConstant,
		AABBattributeBuffer,
		VertexBuffers,
		Count
	};

	//���[�J�����[�g�V�O�l�`���̎��
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
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;


private://�\���̐錾

public://�ÓI�����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="cmdList">�R�}���h���X�g</param>
	static void StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �C���X�^���X����
	/// </summary>
	/// <returns>�C���X�^���X</returns>
	static std::unique_ptr<RaytracingManager> Create();

	/// <summary>
	/// �������
	/// </summary>
	static void Finalize();

private://�ÓI�����o�֐�

	/// <summary>
	/// ���C�g���[�V���O�f�o�C�X�ƃR�}���h���X�g�̍쐬
	/// </summary>
	static void CreateRaytracingInterfaces();

	/// <summary>
	/// ���C�g���[�V���O�p���[�g�V�O�l�`���̐���
	/// </summary>
	/// <param name="desc">�V�O�l�`���ݒ�</param>
	/// <param name="rootSig"></param>
	static void SerializeAndCreateRaytracingRootSignature(
		D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootSig);

	/// <summary>
	/// ���[�g�V�O�l�`���̐���
	/// </summary>
	static void CreateRootSignatures();

	/// <summary>
	/// ???????????????????????
	/// </summary>
	/// <param name="raytracingPipeline">�p�C�v���C��</param>
	static void CreateDxilLibrarySubobject(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);

	/// <summary>
	/// �������W�I���g���ƌ�������Ƃ��Ɏ��s�����ł��߂��q�b�g����ь����V�F�[�_�[���w��
	/// </summary>
	/// <param name="raytracingPipeline">�p�C�v���C��</param>
	static void CreateHitGroupSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);

	/// <summary>
	/// ���[�J�����[�g�����ƃV�F�[�_�[�̊֘A�t��
	/// </summary>
	/// <param name="raytracingPipeline">�p�C�v���C��</param>
	static void CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="desc"></param>
	static void PrintStateObjectDesc(const D3D12_STATE_OBJECT_DESC* desc);

	/// <summary>
	/// ���C�g���[�V���O�p�C�v���C����ԃI�u�W�F�N�g�iRTPSO�j�̍쐬
	/// </summary>
	static void CreateRaytracingPipelineStateObject();

	/// <summary>
	/// �f�X�N���v�^�q�[�v����
	/// </summary>
	static void CreateDescriptorHeap();

private://�����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

public://�����o�֐�


private://�ϐ�

	// �f�o�C�X
	static ID3D12Device* device;
	// �R�}���h���X�g
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