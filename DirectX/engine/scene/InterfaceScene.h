#pragma once
#include <DirectXMath.h>
#include "Camera.h"
#include "Audio.h"

//�I�u�W�F�N�g�n
#include "InterfaceObject3d.h"
#include "Object3d.h"
#include "Sprite.h"
#include "Emitter.h"
#include "LightGroup.h"
//#include "Fbx.h"
#include "Easing.h"
//#include "DrawLine.h"
#include "DrawLine3D.h"
#include "CubeMap.h"
#include "HeightMap.h"
#include "PrimitiveObject3D.h"
#include "InstanceObject.h"

#include "GraphicsPipelineManager.h"

//�v�Z�V�F�[�_�[
#include "ComputeShaderManager.h"

class InterfaceScene
{
protected:// �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	virtual ~InterfaceScene() {};

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void CameraUpdate(Camera* camera) {};

	/// <summary>
	/// �`��
	/// </summary>
	virtual void DrawNotPostB() {};

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw() {};

	/// <summary>
	/// �`��
	/// </summary>
	virtual void DrawNotPostA() {};

	/// <summary>
	/// ���
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// imgui�̕\��
	/// </summary>
	virtual void ImguiDraw() {};

	/// <summary>
	/// �R���s���[�g�V�F�[�_�[����̌v�Z���ʎ擾
	/// </summary>
	virtual void GetConstbufferNum() {};

	/// <summary>
	/// �R�}���h���X�g�̃Z�b�g
	/// </summary>
	/// <param name="inCmdList">�R�}���h���X�g</param>
	void SetCmdList(ID3D12GraphicsCommandList* inCmdList) { cmdList = inCmdList; }

	/// <summary>
	/// ���C�g�̃Z�b�g
	/// </summary>
	/// <param name="light">���C�g�N���X�̃C���X�^���X</param>
	void SetLight(LightGroup* light) { this->light = light; }

protected:

	//�R�}���h���X�g
	ID3D12GraphicsCommandList* cmdList = nullptr;
	//���C�g
	LightGroup* light = nullptr;
};