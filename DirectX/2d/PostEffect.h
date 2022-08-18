#pragma once
#include "Sprite.h"

class PostEffect : public Sprite
{
public: // �T�u�N���X

	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferData
	{
		XMFLOAT4 outlineColor;//�A�E�g���C���̐F
		float outlineWidth;//�A�E�g���C���̑���
	};

	enum TEX_TYPE
	{
		NORMAL,
		BLOOM,
		OUTLINE,
		DEPTH,
		SIZE
	};

public://�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// �C���X�^���X�̐���
	/// </summary>
	static std::unique_ptr<PostEffect> Create();

	/// <summary>
	/// �������
	/// </summary>
	void Finalize();

public://�����o�֐�

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	PostEffect();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~PostEffect();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �[�x�o�b�t�@����
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// �`��R�}���h�̔��s
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

	static void SetPipeline(GraphicsPipelineManager::GRAPHICS_PIPELINE pipe) { pipeline = pipe; }

private://�ÓI�����o�ϐ�

	//��ʃN���A�J���[
	static const float clearColor[2][4];
	//�p�C�v���C��
	static GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline;

private://�����o�ϐ�

	//�e�N�X�`�����
	std::array<std::unique_ptr<Texture>, TEX_TYPE::SIZE> texture;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
};