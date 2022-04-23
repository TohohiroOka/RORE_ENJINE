#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

class BLOB_OBJECT
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	BLOB_OBJECT() {};

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	~BLOB_OBJECT() {
		vs.Reset();
		ps.Reset();
		gs.Reset();
		cs.Reset();
	};

public:
	// ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> vs;
	// �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> ps;
	// �W�I���g���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> gs;
	// �R���s���[�g�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> cs;
};

class ShaderManager
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	ShaderManager() {};
	~ShaderManager() {};

	/// <summary>
	/// �C���X�^���X�̐���
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<ShaderManager> Create();

	/// <summary>
	/// �V�F�[�_�[�̓ǂݍ���
	/// </summary>
	void LoadShader();

	/// <summary>
	/// �������
	/// </summary>
	void Finalize();

public:

	//Obj�̃V�F�[�_�[�I�u�W�F�N�g
	std::unique_ptr<BLOB_OBJECT> obj = nullptr;
	//Sprite�̃V�F�[�_�[�I�u�W�F�N�g
	std::unique_ptr<BLOB_OBJECT> sprite = nullptr;
	//Fbx�̃V�F�[�_�[�I�u�W�F�N�g
	std::unique_ptr<BLOB_OBJECT> fbx = nullptr;
	//DrawLine2d�̃V�F�[�_�[�I�u�W�F�N�g
	std::unique_ptr<BLOB_OBJECT> drawLine2d = nullptr;
	//DrawLine3d�̃V�F�[�_�[�I�u�W�F�N�g
	std::unique_ptr<BLOB_OBJECT> drawLine3d = nullptr;
	//Fbx�̃V�F�[�_�[�I�u�W�F�N�g
	std::unique_ptr<BLOB_OBJECT> postEffect = nullptr;
	//NormalMap�̃V�F�[�_�[�I�u�W�F�N�g
	std::unique_ptr<BLOB_OBJECT> normalMap = nullptr;
	//Particle�̃V�F�[�_�[�I�u�W�F�N�g
	std::unique_ptr<BLOB_OBJECT> particle = nullptr;
	//Compute�̃V�F�[�_�[�I�u�W�F�N�g
	std::unique_ptr<BLOB_OBJECT> Compute = nullptr;
};

