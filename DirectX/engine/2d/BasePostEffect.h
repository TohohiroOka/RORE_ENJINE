#pragma once
#include "Sprite.h"

class BasePostEffect :public Sprite
{
private:

	struct CONST_BUFFER_DATA_BLOOM {
		float strength;//�u���[���̋���
		XMFLOAT4 pad;
	};

	struct CONST_BUFFER_DATA_OUTLINE {
		float outlineWidth;//�A�E�g���C���̑���
		XMFLOAT4 pad;
	};

public:

	enum class EffectTyep {
		bloom,
		outline,
		size,
	};

private:

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �[�x�o�b�t�@����
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// �萔�o�b�t�@����
	/// </summary>
	void CreateConstBuffer(const EffectTyep _type);

public://�����o�֐�

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	BasePostEffect();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~BasePostEffect();

	/// <summary>
	/// �C���X�^���X�̐���
	/// </summary>
	static std::unique_ptr<BasePostEffect> Create(const EffectTyep _type);

	/// <summary>
	/// �`��R�}���h�̔��s
	/// </summary>
	void Draw(const Texture* _tex);

	/// <summary>
	/// �`��O����
	/// </summary>
	void PreDrawScene();

	/// <summary>
	/// �`��㏈��
	/// </summary>
	void PostDrawScene();

	Texture* GetTex() {
		return texture.get();
	}

private://�ÓI�����o�ϐ�

	//��ʃN���A�J���[
	static const float clearColor[4];
	//�p�C�v���C�����
	static std::vector<GraphicsPipelineManager::DrawSet> pipeline;

private://�����o�ϐ�

	//�e�N�X�`�����
	std::unique_ptr<Texture> texture;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuffer;
	//�G�t�F�N�g���
	EffectTyep type;

	//�u���[���̋���
	float strength;
	//�A�E�g���C���̑���
	float outlineWidth;

public:
	static void SetPipeline(const std::vector<GraphicsPipelineManager::DrawSet>& _pipeline) { BasePostEffect::pipeline = _pipeline; }
	void SetStrength(const float _strength) { strength = _strength; }
	void SetOutlineWidth(const float _outlineWidth) { outlineWidth = _outlineWidth; }

};