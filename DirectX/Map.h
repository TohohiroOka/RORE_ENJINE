#pragma once
#include <DirectXMath.h>
#include "PrimitiveObject3D.h"
#include "InstanceObject.h"
#include "Object3d.h"
#include <vector>
#include "CollisionManager.h"
#include <Object3d.h>

class Map
{
private:// �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMINT3 = DirectX::XMINT3;
	using XMINT4 = DirectX::XMINT4;

	enum TYPE {
		NONE,
		NORMAL,
	};

	struct BOX_INFO {
		TYPE type = TYPE::NONE;
		XMFLOAT3 pos;
	};

	//�ݒu�\�ꏊ�̖�
	struct FACE_INFO {
		bool isDraw = false;
		XMFLOAT3 pos;
		XMFLOAT3 rota;
		XMFLOAT4 color = { 1.0f,1.0f ,1.0f ,1.0f };
		XMMATRIX worldMat;
	};

	struct FACE_CHIP {
		XMFLOAT3 mapNum; 
		std::array<FACE_INFO,3> face;
	};
	
public:

	Map() {};
	~Map() {};

	/// <summary>
	/// 3D�I�u�W�F�N�g����
	/// </summary>
	/// <param name="_delimitSize">��؂�̑傫��</param>
	/// <param name="_delimitNum">��؂�̐�</param>
	/// <returns></returns>
	static std::unique_ptr<Map> Create(const float _delimitSize = 10, const XMINT3& _delimitNum = { 10,10,10 });

	/// <summary>
	/// ��؂���ύX
	/// </summary>
	/// <param name="_delimitNum"></param>
	void ChangeDelimitNum(const XMINT3& _delimitNum = { 5,5,5 });

	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="cameraTarget">�J�����̃^�[�Q�b�g</param>
	void Update(const XMFLOAT3& _pos, const XMFLOAT3& cameraTarget);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// �`��
	/// </summary>
	void InstanceDraw();

	/// <summary>
	/// �}�b�v�ǂݍ���
	/// </summary>
	/// <param name="_fileName">�t�@�C����</param>
	void LoadMap(const std::string _fileName);

	/// <summary>
	/// �{�b�N�X�̐ݒu
	/// </summary>
	void AddBox();

	/// <summary>
	/// �}�b�v�o��
	/// </summary>
	/// <param name="_fileName">�t�@�C����</param>
	/// <param name="_cameraDist">�J��������</param>
	void OutputMap(const std::string& _fileName,float _cameraDist);

private:

	/// <summary>
	/// ���[���h�s��
	/// </summary>
	/// <param name="_pos">���W</param>
	/// <param name="_scale">�T�C�Y</param>
	/// <param name="_rotation">��]</param>
	/// <param name="_worldMat">���[���h�s��</param>
	void WorldMat(const XMFLOAT3& _pos, const XMFLOAT3& _scale, const XMFLOAT3& _rotation, XMMATRIX& _worldMat);

	/// <summary>
	/// ���C�ƃ{�b�N�X�̔���
	/// </summary>
	/// <param name="_point1">���W</param>
	/// <param name="_point2">���W</param>
	/// <param name="_boxMat">���[���h�s��</param>
	/// <returns>�Փˋ���</returns>
	float ColRayBox(const XMFLOAT3& _point1, const XMFLOAT3& _point2, const XMMATRIX& _boxMat);

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_delimitSize">��؂�̑傫��</param>
	/// <param name="_delimitNum">��؂�̐�</param>
	void Initialize(const float _delimitSize, const XMINT3& _delimitNum);

	/// <summary>
	/// ������
	/// </summary>
	void CreateLine();

	/// <summary>
	/// �ʐ���
	/// </summary>
	void CreateFace();

	/// <summary>
	/// �z�u�ꏊ�ړ�
	/// </summary>
	/// <param name="_angle">�J�����p�x</param>
	void Move(const float _angle);

private:

	//��؂�T�C�Y
	float delimitSize;
	//��؂��
	XMINT3 delimitNum;
	//���̏��z��
	std::vector<float> linePosList_x;
	std::vector<float> linePosList_y;
	std::vector<float> linePosList_z;
	//��
	std::unique_ptr<PrimitiveObject3D> lineObject = nullptr;
	//stage�̍L���ύX�t���O
	bool isChange;

	//�ݒu�\�ꏊ�̖�
	std::unique_ptr<Model> faceModel;
	std::array<std::unique_ptr<InstanceObject>, 15> faceObject;
	std::vector<std::vector<std::vector<FACE_CHIP>>> faceInfo;

	//map�̃{�b�N�X
	std::unique_ptr<Model> boxModel;
	std::unique_ptr<InstanceObject> boxObject;
	std::vector<std::vector<std::vector<BOX_INFO>>> boxInfo;

	//�}�E�X�̃��C���
	Ray mouseLay;

	//���݂̎w���
	XMINT4 pointface;
};

