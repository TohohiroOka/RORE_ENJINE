#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include "ShaderManager.h"

#include <unordered_map>

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
		SCALE,
		COLOR,
	};

	//�p�C�v���C���ݒ�
	struct PEPELINE_DESC
	{
		//2D�`�悩
		bool object2d = false;
		//�p�[�e�B�N���`�悩
		bool particl = false;
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
		//�}�e���A���f�[�^�̗L��
		bool materialData = true;
		//�C���X�^���V���O�`��
		bool instanceDraw = false;
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
	/// <param name="_mode">�u�����h�̎��</param>
	/// <returns>�u�����h�ݒ�</returns>
	D3D12_RENDER_TARGET_BLEND_DESC CreateBlendDesc(const BLEND_MODE& _mode);

	/// <summary>
	/// �p�C�v���C���f�X�N�̐���
	/// </summary>
	/// <param name="_pepelineDescSet">�p�C�v���C���ݒ�</param>
	/// <returns>�p�C�v���C���f�X�N</returns>
	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatepelineDesc(const PEPELINE_DESC& _pepelineDescSet);

	/// <summary>
	/// ���[�g�V�O�l�`���̐���
	/// </summary>
	/// <param name="_signatureDescSet">���[�g�V�O�l�`���ݒ�</param>
	void CreateRootSignature(const SIGNATURE_DESC& _signatureDescSet);

public://�����o�֐�

	GraphicsPipelineManager();
	~GraphicsPipelineManager();

	/// <summary>
	/// �f�o�C�X�̃Z�b�g
	/// </summary>
	/// <param name="_device">�f�o�C�X</param>
	static void SetDevice(ID3D12Device* _device) { GraphicsPipelineManager::device = _device; }

	/// <summary>
	/// �p�C�v���C���̐���
	/// enum OBJECT_KINDS�̒������������Ƃ���
	/// </summary>
	/// <param name="_name">�p�C�v���C����</param>
	/// <param name="_pepelineDescSet">�p�C�v���C���̐ݒ�</param>
	/// <param name="_signatureDescSet">���[�g�V�O�l�`���ݒ�</param>
	void CreatePipeline(const std::string& _name, const PEPELINE_DESC& _pepelineDescSet, const SIGNATURE_DESC& _signatureDescSet);

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
	std::unordered_map<std::string, GRAPHICS_PIPELINE> graphicsPipeline;

};