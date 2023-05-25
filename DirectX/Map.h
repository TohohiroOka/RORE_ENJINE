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
private:// エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMINT3 = DirectX::XMINT3;
	using XMINT4 = DirectX::XMINT4;

	enum class TYPE {
		NONE,
		PLAYER,
		GOAL,
		NORMAL,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		FRONT,
		BACK,
		SIZE,
	};

	enum class BOX_TYPE {
		NORMAL,
		FACE,
		SIZE,
	};

	struct BOX_INFO {
		TYPE type = TYPE::NONE;
		XMFLOAT3 pos;
		XMMATRIX worldMat;
	};

	//設置可能場所の面
	struct FACE_INFO {
		bool isPossibleHit = false;
		bool isEdge = false;
		XMFLOAT3 pos;
		XMFLOAT3 rota;
		XMFLOAT4 color = { 1.0f,1.0f ,1.0f ,1.0f };
		XMMATRIX worldMat;
	};

	struct FACE_CHIP {
		XMFLOAT3 mapNum; 
		std::array<FACE_INFO,3> face;
	};
	
	struct HIT_INFO {
		float dist;
		XMVECTOR hitPos;
	};

public:

	Map() {};
	~Map();

	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <param name="_delimitSize">区切りの大きさ</param>
	/// <param name="_delimitNum">区切りの数</param>
	/// <returns></returns>
	static std::unique_ptr<Map> Create(const float _delimitSize = 10, const XMINT3& _delimitNum = { 7,7,7 });

	/// <summary>
	/// 区切り個数変更
	/// </summary>
	/// <param name="_frontBack">前か後ろに追加/true->後ろ/false->前</param>
	/// <param name="_delimitNum"></param>
	void ChangeDelimitNum(const bool _frontBack = true, const XMINT3& _delimitNum = { 5,5,5 });

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_cameraTarget">カメラのターゲット</param>
	/// <param name="_kaburi">imguiと被っているか</param>
	/// <param name="_isEdgeHit">外側の判定を取るか</param>
	void Update(const XMFLOAT3& _pos, const XMFLOAT3& _cameraTarget, const bool _kaburi, const bool _isOutsideCollision);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 描画
	/// </summary>
	void InstanceDraw();

	/// <summary>
	/// ボックスの設置
	/// </summary>
	/// <param name="_cameraPos">カメラ座標</param>
	void AddBox(const XMFLOAT3& _cameraPos, const std::array<bool, 9> _isSetObject);

	/// <summary>
	/// ボックスの削除
	/// </summary>
	/// <param name="_cameraPos">カメラ座標</param>
	void DeleteBox(const XMFLOAT3& _cameraPos);

	/// <summary>
	/// マップ出力
	/// </summary>
	/// <param name="_fileName">ファイル名</param>
	/// <param name="_cameraDist">カメラ距離</param>
	void OutputMap(const std::string& _fileName,float _cameraDist);

	/// <summary>
	/// マップ入力
	/// </summary>
	bool ImputMap(const std::string& _fileName);

	/// <summary>
	/// 
	/// </summary>
	void FrameReset();

	/// <summary>
	/// rightのon/off
	/// </summary>
	void SetLight(const bool set);

	/// <summary>
	/// 1つ戻る
	/// </summary>
	void Undo();

	/// <summary>
	/// 1つ進む
	/// </summary>
	void Redo();

private:

	/// <summary>
	/// ワールド行列
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="_scale">サイズ</param>
	/// <param name="_rotation">回転</param>
	/// <param name="_worldMat">ワールド行列</param>
	void WorldMat(const XMFLOAT3& _pos, const XMFLOAT3& _scale, const XMFLOAT3& _rotation, XMMATRIX& _worldMat);

	/// <summary>
	/// レイとボックスの判定
	/// </summary>
	/// <param name="_point1">座標</param>
	/// <param name="_point2">座標</param>
	/// <param name="_boxMat">ワールド行列</param>
	/// <param name="_hitInfo">交差情報</param>
	/// <returns>衝突したか</returns>
	bool ColRayBox(const XMFLOAT3& _point1, const XMFLOAT3& _point2, const XMMATRIX& _boxMat, HIT_INFO* _hitInfo);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_delimitSize">区切りの大きさ</param>
	/// <param name="_delimitNum">区切りの数</param>
	void Initialize(const float _delimitSize, const XMINT3& _delimitNum);

	/// <summary>
	/// 線生成
	/// </summary>
	void CreateLine();

	/// <summary>
	/// 面生成
	/// </summary>
	void CreateFace();

	/// <summary>
	/// import時の面生成
	/// </summary>
	void ImportFace();

	/// <summary>
	/// binaryの出力
	/// </summary>
	void BinaryOutput();

private:

	//区切りサイズ
	float delimitSize;
	//区切り個数
	XMINT3 delimitNum;
	//線の情報配列
	std::vector<float> linePosList_x;
	std::vector<float> linePosList_y;
	std::vector<float> linePosList_z;
	//線
	std::unique_ptr<PrimitiveObject3D> lineObject = nullptr;
	//stageの広さ変更フラグ
	bool isChange;

	//設置可能場所の面
	std::unique_ptr<Model> faceModel;
	std::unique_ptr<Model> faceModel2;
	std::unique_ptr<InstanceObject> faceObject;
	std::vector<std::vector<std::vector<FACE_CHIP>>> faceInfo;

	//mapのボックス
	std::array<std::unique_ptr<Model>, int(BOX_TYPE::SIZE)> boxModel;
	std::array<std::unique_ptr<InstanceObject>, int(BOX_TYPE::SIZE)> boxObject;
	std::vector<std::vector<std::vector<BOX_INFO>>> boxInfo;

	//マウスのレイ情報
	Ray mouseLay;

	//現在の指定面
	XMINT4 pointface;
	//接触座標
	HIT_INFO hitInfo;
	//設置ボックスとレイが当たっているか
	bool isHitBox;

	//現在の手順
	int orderNum;
	//現在の最大手順
	int orderMaxNum;
	//消去時の最大番号
	int deleteOrderMaxNum;

public:

	XMINT3 GetDelimitNum() { return delimitNum; }
	float GetDelimitSize() { return delimitSize; }
};

