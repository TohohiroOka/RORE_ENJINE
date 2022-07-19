#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include "ShaderManager.h"

class GraphicsPipelineManager
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://�����oenum

	struct GRAPHICS_PIPELINE
	{
		// ���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature> rootSignature = nullptr;
		// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
		ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	};

	//�u�����h���[�h
	enum BLEND_MODE
	{
		NOBLEND,//�f�t�H���g�ݒ�
		ALPHA,//������
		ADD,//���Z����
		SUB,//���Z����
		MULA,//��Z����
		INVSRC,//���]����
	};

	//���_���C�A�E�g�����o�[
	enum INPUT_LAYOUT_NUMBER
	{
		POSITION,
		NORMAL,
		TEXCOORD_2D,
		TEXCOORD_3D,
		BONEINDICES,
		BONEWEIGHTS,
	};

	//�p�C�v���C���ݒ�
	struct PEPELINE_DESC
	{
		//2D�`�悩
		bool object2d = false;
		//���_�V�F�[�_�[
		std::string vertShader = "null";
		//�s�N�Z���V�F�[�_�[
		std::string pixelShader = "null";
		//�W�I���g���V�F�[�_�[
		std::string geometryShader = "null";
		//���C�A�E�g��
		int layoutNum = 0;
		//���_���C�A�E�g�ݒ�
		D3D12_INPUT_ELEMENT_DESC* inputLayout = nullptr;
		//�u�����h�X�e�[�g�ݒ萔
		int stateNum = 1;
		//�u�����h�̎��
		BLEND_MODE blendMode = BLEND_MODE::ALPHA;
		//�`����@
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//�����_�[�^�[�Q�b�g��
		int rtvNum = 1;
	};

	//���[�g�V�O�l�`���ݒ�
	struct SIGNATURE_DESC
	{
		//2D�`�悩
		bool object2d = false;
		//�e�N�X�`����
		int textureNum = 1;
		//���C�g�L��
		bool light = true;
		//�L���[�u�}�b�v�L��
		bool cubemap = false;
	};

private://�����o�֐�

	/// <summary>
	/// �u�����h���[�h�̐ݒ�
	/// </summary>
	/// <param name="mode">�u�����h�̎��</param>
	/// <returns>�u�����h�ݒ�</returns>
	D3D12_RENDER_TARGET_BLEND_DESC CreateBlendDesc(BLEND_MODE mode);

	/// <summary>
	/// �p�C�v���C���f�X�N�̐���
	/// </summary>
	/// <param name="pepelineDescSet">�p�C�v���C���ݒ�</param>
	/// <returns>�p�C�v���C���f�X�N</returns>
	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatepelineDesc(PEPELINE_DESC* pepelineDescSet);

	/// <summary>
	/// ���[�g�V�O�l�`���̐���
	/// </summary>
	/// <param name="signatureDescSet">���[�g�V�O�l�`���ݒ�</param>
	void CreateRootSignature(SIGNATURE_DESC* signatureDescSet);

public://�����o�֐�

	GraphicsPipelineManager();
	~GraphicsPipelineManager();

	/// <summary>
	/// �f�o�C�X�̃Z�b�g
	/// </summary>
	/// <param name="device">�f�o�C�X��</param>
	static void SetDevice(ID3D12Device* device) { GraphicsPipelineManager::device = device; }

	/// <summary>
	/// �p�C�v���C���̐���
	/// enum OBJECT_KINDS�̒������������Ƃ���
	/// </summary>
	/// <param name="className">�p�C�v���C����</param>
	/// <param name="pepelineDescSet">�p�C�v���C���̐ݒ�</param>
	/// <param name="signatureDescSet">���[�g�V�O�l�`���ݒ�</param>
	void CreatePipeline(const std::string name, PEPELINE_DESC* pepelineDescSet, SIGNATURE_DESC* signatureDescSet);

private://�ÓI�����o�ϐ�

	// �f�o�C�X
	static ID3D12Device* device;

private://�����o�ϐ�

	//�V�F�[�_�[
	std::unique_ptr<ShaderManager> shaderManager;

public://�����o�ϐ�

	//���O�̕ێ�
	std::string name;
	//�p�C�v���C���ۑ��z��
	std::map<std::string, GRAPHICS_PIPELINE> graphicsPipeline;

};