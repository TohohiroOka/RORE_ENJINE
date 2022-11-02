#pragma once
#include "InterfaceObject3d.h"

class BaseCollider;
class Camera;
class LightGroup;

class PrimitiveObject3D : public InterfaceObject3d
{
public://�\���̐錾

	//�萔�o�b�t�@�̍\����
	struct CONST_BUFFER_DATA {
		XMFLOAT4 color;//�F
		XMMATRIX matWorld;//3D�ϊ��s��
		XMMATRIX viewproj;//3D�ϊ��s��
	};

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
	/// �p�C�v���C���̃Z�b�g
	/// </summary>
	/// <param name="_pipeline">�p�C�v���C��</param>
	static void SetPipeline(const GraphicsPipelineManager::GRAPHICS_PIPELINE& _pipeline) { PrimitiveObject3D::pipeline = _pipeline; }

private:

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

public://�����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

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
	std::vector<unsigned long> indices;
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;
	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView{};
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;

public:

	/// <summary>
	/// ���_�̃Z�b�g
	/// </summary>
	/// <param name="_vertex">���_</param>
	void SetVertex(const XMFLOAT3& _vertex) { this->vertices.push_back(_vertex); }

	/// <summary>
	/// ���_�̃��Z�b�g
	/// </summary>
	void ResetVertex() { 
		this->vertices.clear();
		this->vertices.resize(0);
	}

	/// <summary>
	/// ���_�̐�
	/// </summary>
	int VertexNum() { return int(vertices.size()); }

};