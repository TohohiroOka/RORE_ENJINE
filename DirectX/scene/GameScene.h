#pragma once
#include <DirectXMath.h>
#include <memory>

#include "Audio.h"
#include "Object3d.h"
#include "Sprite.h"
#include "Emitter.h"
#include "LightGroup.h"
#include "Fbx.h"
#include "Easing.h"
#include "NormalMap.h"
#include "DrawLine.h"
#include "DrawLine3D.h"
#include "ComputeShaderManager.h"

#include "Player.h"
#include "Ground.h"
#include "TouchableObject.h"

class DirectInput;
class XInputManager;
class Audio;
class Camera;

class GameScene
{
private:// �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://�ÓI�����o�֐�

	/// <summary>
	/// �C���X�^���X�̐���
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<GameScene> Create();

public:// �����o�֐�

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	GameScene() {};

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~GameScene() {};

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	/// <param name="camera">Camera�̃C���X�^���X</param>
	void Update(Camera* camera);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �R���s���[�g�V�F�[�_�[����̌v�Z���ʎ擾
	/// </summary>
	void GetConstbufferNum();

private:// �����o�ϐ�

	//��
	std::unique_ptr<Audio> audio = nullptr;

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

	//NormalMap
	std::unique_ptr<NormalMap> water = nullptr;
	int tex[3];
	bool lightF = false;
	float lightPos = 0;
	float uvPos = 0;

	//���C�g
	std::unique_ptr<LightGroup> light = nullptr;

	//FbxModel
	std::unique_ptr<FbxModel> danceModel = nullptr;

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