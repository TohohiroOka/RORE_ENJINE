#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"

class TestField : public InterfaceScene
{
public:

	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

	/// <summary>
	///	�X�V
	/// </summary>
	void Update() override;

	/// <summary>
	///	�`��
	/// </summary>
	void Draw() override;

	/// <summary>
	///	���
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// imgui�̕\��
	/// </summary>
	void ImguiDraw() override;

	/// <summary>
	/// �R���s���[�g�V�F�[�_�[����̌v�Z���ʎ擾
	/// </summary>
	void GetConstbufferNum() override;

private:

	//�J�����̉�]
	float cameraAngle = 100;
	//�J�����̍���
	float cameraY = 50.0f;

	//�X�v���C�g
	std::unique_ptr<Sprite> sprite = nullptr;

	//�p�[�e�B�N��
	std::unique_ptr<Emitter> emit = nullptr;

	//���f��
	std::unique_ptr<Model> uma = nullptr;
	std::unique_ptr<Model> ground = nullptr;
	std::unique_ptr<Model> block = nullptr;

	//����N���X
	std::unique_ptr<Player> PLAYER = nullptr;
	std::unique_ptr<Ground> GROUND = nullptr;

	//Obj
	std::unique_ptr<Object3d> BLOCK = nullptr;

	//FbxModel
	std::unique_ptr<FbxModel> danceModel = nullptr;
	std::unique_ptr<FbxModel> SpherePBRModel = nullptr;
	std::unique_ptr<FbxModel> SpiralPBRModel = nullptr;

	//Fbx
	std::unique_ptr<Fbx> anm = nullptr;

	//�J�������W
	XMFLOAT3 cameraPosition = { 0,0,50 };

	//��
	std::unique_ptr<DrawLine> line = nullptr;
	std::unique_ptr<DrawLine> line_t = nullptr;
	std::unique_ptr<DrawLine3D> line3d = nullptr;

	//�R���s���[�g�V�F�[�_�[
	std::unique_ptr<ComputeShaderManager> compute = nullptr;
	static const int max = 10;

	XMFLOAT3 startPosition[max];//�������W
	XMFLOAT3 endPosition[max];//�I�����W
	XMFLOAT3 nowPosition[max];//���ݍ��W
	float time[max];//����

};

