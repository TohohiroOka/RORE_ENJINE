#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

class DirectXCommon;

/// <summary>
/// �X�v���C�g
/// </summary>
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
	struct VertexPosUv
	{
		XMFLOAT3 pos; // xyz���W
		XMFLOAT2 uv;  // uv���W
	};

	/// <summary>
	/// �萔�o�b�t�@�p�f�[�^�\����
	/// </summary>
	struct ConstBufferData
	{
		XMFLOAT4 color;	// �F (RGBA)
		XMMATRIX mat;	// �R�c�ϊ��s��
	};

public: // �ÓI�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <returns>����</returns>
	static bool StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="texnumber">�e�N�X�`���ԍ�</param>
	/// <param name="filename">�摜�t�@�C����</param>
	/// <returns>����</returns>
	static bool LoadTexture(UINT texnumber, const wchar_t* filename);

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// �X�v���C�g����
	/// </summary>
	/// <returns>Sprite</returns>
	static Sprite* Create();

protected: // �ÓI�����o�ϐ�

	// �e�N�X�`���̍ő喇��
	static const int srvCount = 512;
	// ���_��
	static const int vertNum = 4;
	// �f�o�C�X
	static ID3D12Device* device;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootSignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelineState;
	// �ˉe�s��
	static XMMATRIX matProjection;
	// �f�X�N���v�^�q�[�v
	static ComPtr<ID3D12DescriptorHeap> descHeap;
	// �e�N�X�`���o�b�t�@
	static ComPtr<ID3D12Resource> texBuff[srvCount];

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
	/// <returns>����</returns>
	bool Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

protected: // �����o�ϐ�

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// �e�N�X�`���ԍ�
	UINT texNumber = 0;
	// Z�����̉�]�p
	float rotation = 0.0f;
	// ���W
	XMFLOAT2 position{ 0, 0 };
	// �X�v���C�g���A����
	XMFLOAT2 scale = { 100.0f, 100.0f };
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
	/// �ʒu�擾
	/// </summary>
	/// <returns>position</returns>
	const XMFLOAT2& GetPosition() { return position; }

	/// <summary>
	/// ��]�p�擾
	/// </summary>
	/// <returns>rotation</returns>
	const float GetRotation() { return rotation; }

	/// <summary>
	/// �傫���擾
	/// </summary>
	/// <returns>scale</returns>
	const XMFLOAT2& GetScale() { return scale; }

	/// <summary>
	/// �e�N�X�`�����擾
	/// </summary>
	/// <returns>texSize</returns>
	const XMFLOAT2& GetTexSize() { return texSize; }

	/// <summary>
	/// �e�N�X�`���n�_�擾
	/// </summary>
	/// <returns>texLeftTop</returns>
	const XMFLOAT2& GetTexLeftTop() { return texLeftTop; }

	/// <summary>
	/// �F�擾
	/// </summary>
	/// <returns>color</returns>
	const XMFLOAT4& GetColor() { return color; }

	/// <summary>
	/// �A���J�[�|�C���g�擾
	/// </summary>
	/// <returns>anchorpoint</returns>
	const XMFLOAT2& GetAnchorpoint() { return anchorpoint; }

	/// <summary>
	/// ���E���]�擾
	/// </summary>
	/// <returns>isFlipX</returns>
	const bool GetIsFlipX() { return isFlipX; }

	/// <summary>
	/// �㉺���]�擾
	/// </summary>
	/// <returns>isFlipY</returns>
	const bool GetIsFlipY() { return isFlipY; }

	/// <summary>
	/// �e�N�X�`���ԍ��Z�b�g
	/// </summary>
	/// <param name="texNumber">�e�N�X�`���ԍ�</param>
	void SetTexNumber(UINT texNumber) { this->texNumber = texNumber; };

	/// <summary>
	/// �ʒu�Z�b�g
	/// </summary>
	/// <param name="position">�ʒu</param>
	void SetPosition(const XMFLOAT2& position) { this->position = position; }

	/// <summary>
	/// ��]�p�Z�b�g
	/// </summary>
	/// <param name="rotation">��]�p</param>
	void SetRotation(float rotation) { this->rotation = rotation; }

	/// <summary>
	/// �傫���Z�b�g
	/// </summary>
	/// <param name="scale">�傫��</param>
	void SetScale(const XMFLOAT2& scale) { this->scale = scale; }

	/// <summary>
	/// �e�N�X�`���̕��Z�b�g
	/// </summary>
	/// <param name="texSize">�e�N�X�`���̕�</param>
	void SetTexSize(const XMFLOAT2& texSize) { this->texSize = texSize; }

	/// <summary>
	/// �e�N�X�`���n�_�Z�b�g
	/// </summary>
	/// <param name="texLeftTop">�e�N�X�`���n�_</param>
	void SetTexLeftTop(const XMFLOAT2& texLeftTop) { this->texLeftTop = texLeftTop; }

	/// <summary>
	/// �F�Z�b�g
	/// </summary>
	/// <param name="color">�F</param>
	void SetColor(const XMFLOAT4& color) { this->color = color; }

	/// <summary>
	/// �A���J�[�|�C���g�Z�b�g
	/// </summary>
	/// <param name="anchorpoint">�A���J�[�|�C���g</param>
	void SetAnchorpoint(const XMFLOAT2& anchorpoint) { this->anchorpoint = anchorpoint; }

	/// <summary>
	/// ���E���]�Z�b�g
	/// </summary>
	/// <param name="isFlipX">���E���]</param>
	void SetIsFlipX(bool isFlipX) { this->isFlipX = isFlipX; };

	/// <summary>
	/// �㉺���]�Z�b�g
	/// </summary>
	/// <param name="isFlipY">�㉺���]</param>
	void SetIsFlipY(bool isFlipY) { this->isFlipY = isFlipY; };
};