#pragma once
#include "Sprite.h"

class PostEffect : public Sprite
{
private://�ÓI�����o�֐�

	/// <summary>
	/// �O���t�B�b�N�X�p�C�v���C���̐���
	/// </summary>
	static void CreateGraphicsPipeline();

public://�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// �C���X�^���X�̐���
	/// </summary>
	static std::unique_ptr<PostEffect> Create();

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

	/// <summary>
	/// �������
	/// </summary>
	void AllDelete();

private://�ÓI�����o�ϐ�
	
	//�e�N�X�`���o�b�t�@�̐�
	static const int texBuffNum = 2;
	//��ʃN���A�J���[
	static const float clearColor[4];
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootSignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelineState;

private://�����o�ϐ�

	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff[texBuffNum];
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> depthBuff;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
};