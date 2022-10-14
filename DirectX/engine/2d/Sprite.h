#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

#include "GraphicsPipelineManager.h"
#include "Texture.h"

class Sprite
{
protected: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X

	/// <summary>
	/// ���_�f�[�^�\����
	/// </summary>
	struct VERTEX
	{
		XMFLOAT3 pos; // xyz���W
		XMFLOAT2 uv;  // uv���W
	};

	/// <summary>
	/// �萔�o�b�t�@�p�f�[�^�\����
	/// </summary>
	struct CONST_BUFFER_DATA
	{
		XMFLOAT4 color;	// �F (RGBA)
		XMMATRIX mat;	// �R�c�ϊ��s��
	};

	struct INFORMATION
	{
		bool isDelete = false; //�V�[���J�ڂō폜���s����
		std::unique_ptr<Texture> instance = nullptr;
	};

public: // �ÓI�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="_device">�f�o�C�X</param>
	/// <returns>����</returns>
	static bool StaticInitialize(ID3D12Device* _device);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="_keepName">�ۑ���</param>
	/// <param name="_filename">�摜�t�@�C����</param>
	/// <param name="_isDelete">�V�[���J�ڂō폜���s����</param>
	static void LoadTexture(const std::string& _keepName, const std::string& _filename, bool _isDelete = true);

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="_cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* _cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// �X�v���C�g����
	/// </summary>
	/// <param name="_name">�e�N�X�`���ۑ���</param>
	/// <returns>�C���X�^���X</returns>
	static std::unique_ptr<Sprite> Create(const std::string& _name);

	/// <summary>
	/// �V�[�����Ƃ̉������
	/// </summary>
	static void SceneFinalize();

	/// <summary>
	/// �������
	/// </summary>
	static void Finalize();

	/// <summary>
	/// �p�C�v���C���̃Z�b�g
	/// </summary>
	/// <param name="_pipeline">�p�C�v���C��</param>
	static void SetPipeline(const GraphicsPipelineManager::GRAPHICS_PIPELINE& _pipeline) { pipeline = _pipeline; }

protected: // �ÓI�����o�ϐ�

	// ���_��
	static const int vertNum = 4;
	// �f�o�C�X
	static ID3D12Device* device;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	//�p�C�v���C��
	static GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline;
	//�e�N�X�`�����
	static std::map<std::string, INFORMATION> texture;
	// �ˉe�s��
	static XMMATRIX matProjection;

public: // �����o�֐�

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Sprite() {};

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Sprite();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_name">�e�N�X�`���ۑ���</param>
	/// <param name="_anchorpoint">�A���J�[�|�C���g</param>
	/// <param name="_isFlipX">���E���]���邩</param>
	/// <param name="_isFlipY">�㉺���]���邩</param>
	void Initialize(const std::string& _name, const XMFLOAT2& _anchorpoint,bool _isFlipX = false, bool _isFlipY = false);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

protected: // �����o�ϐ�

	//�e�N�X�`����
	std::string name;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// Z�����̉�]�p
	float rotation = 0.0f;
	// ���W
	XMFLOAT2 position{ 0, 0 };
	// �X�v���C�g���A����
	XMFLOAT2 size = { 100.0f, 100.0f };
	// �A���J�[�|�C���g
	XMFLOAT2 anchorpoint = { 0, 0 };
	// ���[���h�s��
	XMMATRIX matWorld{};
	// �F
	XMFLOAT4 color = { 1, 1, 1, 1 };
	// ���E���]
	bool isFlipX = false;
	// �㉺���]
	bool isFlipY = false;
	// �e�N�X�`���n�_
	XMFLOAT2 texLeftTop = { 0, 0 };
	// �e�N�X�`�����A����
	XMFLOAT2 texSize = { 500.0f, 500.0f };

protected: // �����o�֐�

	/// <summary>
	/// ���_�f�[�^�]��
	/// </summary>
	void TransferVertices();

public:

	/// <summary>
	/// ���W�̎擾
	/// </summary>
	/// <returns>���W</returns>
	const XMFLOAT2& GetPosition() { return position; }

	/// <summary>
	/// ��]�p�̎擾
	/// </summary>
	/// <returns>��]�p</returns>
	float GetRotation() { return rotation; }

	/// <summary>
	/// �傫���̎擾
	/// </summary>
	/// <returns>�傫��</returns>
	const XMFLOAT2& GetSize() { return size; }

	/// <summary>
	/// �����̎擾
	/// </summary>
	/// <returns>����</returns>
	const XMFLOAT2& GetTexSize() { return texSize; }

	/// <summary>
	/// �I�_�̎擾
	/// </summary>
	/// <returns>�I�_</returns>
	const XMFLOAT2& GetTexLeftTop() { return texLeftTop; }

	/// <summary>
	/// �F�̎擾
	/// </summary>
	/// <returns>�F</returns>
	const XMFLOAT4& GetColor() { return color; }

	/// <summary>
	/// �A���J�[�|�C���g�̎擾
	/// </summary>
	/// <returns>�A���J�[�|�C���g</returns>
	const XMFLOAT2& GetAnchorpoint() { return anchorpoint; }

	/// <summary>
	/// ���E���]�t���O�̎擾
	/// </summary>
	/// <returns>���E���]�t���O</returns>
	bool GetIsFlipX() { return isFlipX; }

	/// <summary>
	/// �㉺���]�t���O�̎擾
	/// </summary>
	/// <returns>���E���]�t���O</returns>
	bool GetIsFlipY() { return isFlipY; }

	/// <summary>
	/// �e�N�X�`���̃Z�b�g
	/// </summary>
	void SetTexNumber(const std::string& _name) { this->name = _name; };

	/// <summary>
	/// ���W�̓���
	/// </summary>
	void SetPosition(const XMFLOAT2& _position) { this->position = _position; }

	/// <summary>
	/// ��]�p�̓���
	/// </summary>
	void SetRotation(float _rotation) { this->rotation = _rotation; }

	/// <summary>
	/// �傫���̓���
	/// </summary>
	void SetSize(const XMFLOAT2& _size) { this->size = _size; }

	/// <summary>
	/// �����̓���
	/// </summary>
	void SetTexSize(const XMFLOAT2& _texSize) { this->texSize = _texSize; }

	/// <summary>
	/// �I�_�̓���
	/// </summary>
	void SetTexLeftTop(const XMFLOAT2& _texLeftTop) { this->texLeftTop = _texLeftTop; }

	/// <summary>
	/// �F�̓���
	/// </summary>
	void SetColor(const XMFLOAT4& _color) { this->color = _color; }

	/// <summary>
	/// �A���J�[�|�C���g�̓���
	/// </summary>
	void SetAnchorpoint(const XMFLOAT2& _anchorpoint) { this->anchorpoint = _anchorpoint; }

	/// <summary>
	/// ���E���]�t���O�̓���
	/// </summary>
	void SetIsFlipX(bool _isFlipX) { this->isFlipX = _isFlipX; };

	/// <summary>
	/// �㉺���]�t���O�̓���
	/// </summary>
	void SetIsFlipY(bool _isFlipY) { this->isFlipY = _isFlipY; };
};