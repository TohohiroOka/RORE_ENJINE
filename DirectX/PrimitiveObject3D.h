#pragma once
#include "InterfaceObject3d.h"

class BaseCollider;
class Camera;
class LightGroup;

class PrimitiveObject3D : public InterfaceObject3d
{
public://�ÓI�����o�֐�

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~PrimitiveObject3D();

	/// <summary>
	/// �`��O����
	/// </summary>
	static void PreDraw();

	/// <summary>
	/// �p�C�v���C���̐ݒ�
	/// </summary>
	/// <param name="pipeline"></param>
	static void SetPipeline(GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline) { PrimitiveObject3D::pipeline = pipeline; }

public://�����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// //�`��
	/// </summary>
	void Draw() override;

private://�ÓI�����o�ϐ�

	//�p�C�v���C��
	static GraphicsPipelineManager::GRAPHICS_PIPELINE pipeline;

protected://�����o�ϐ�

	// ���_�z��
	std::vector<XMFLOAT3> vertices;
	//�C���f�b�N�X�z��
	std::vector<unsigned short> indices;
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;
	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView{};

public:

	/// <summary>
	/// ���_�̃Z�b�g
	/// </summary>
	/// <param name="model">���_</param>
	void SetVertex(XMFLOAT3 vertex) { this->vertices.push_back(vertex); }
};