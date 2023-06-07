#pragma once
#include "Sprite.h"

class PostEffect : public Sprite
{
public:

	enum class EffectTyep {
		normal,
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
	void Draw(const EffectTyep _drawMode = EffectTyep::normal);

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

public:
	static void SetPipeline(const std::vector<GraphicsPipelineManager::DrawSet>& _pipeline) { PostEffect::pipeline = _pipeline; }

};