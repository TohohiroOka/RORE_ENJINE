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

	//設置可能場所の面
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
	/// 3Dオブジェクト生成
	/// </summary>
	/// <param name="_delimitSize">区切りの大きさ</param>
	/// <param name="_delimitNum">区切りの数</param>
	/// <returns></returns>
	static std::unique_ptr<Map> Create(const float _delimitSize = 10, const XMINT3& _delimitNum = { 10,10,10 });

	/// <summary>
	/// 区切り個数変更
	/// </summary>
	/// <param name="_delimitNum"></param>
	void ChangeDelimitNum(const XMINT3& _delimitNum = { 5,5,5 });

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="_pos">座標</param>
	/// <param name="cameraTarget">カメラのターゲット</param>
	void Update(const XMFLOAT3& _pos, const XMFLOAT3& cameraTarget);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 描画
	/// </summary>
	void InstanceDraw();

	/// <summary>
	/// マップ読み込み
	/// </summary>
	/// <param name="_fileName">ファイル名</param>
	void LoadMap(const std::string _fileName);

	/// <summary>
	/// ボックスの設置
	/// </summary>
	void AddBox();

	/// <summary>
	/// マップ出力
	/// </summary>
	/// <param name="_fileName">ファイル名</param>
	/// <param name="_cameraDist">カメラ距離</param>
	void OutputMap(const std::string& _fileName,float _cameraDist);

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
	/// <returns>衝突距離</returns>
	float ColRayBox(const XMFLOAT3& _point1, const XMFLOAT3& _point2, const XMMATRIX& _boxMat);

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
	/// 配置場所移動
	/// </summary>
	/// <param name="_angle">カメラ角度</param>
	void Move(const float _angle);

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
	std::array<std::unique_ptr<InstanceObject>, 15> faceObject;
	std::vector<std::vector<std::vector<FACE_CHIP>>> faceInfo;

	//mapのボックス
	std::unique_ptr<Model> boxModel;
	std::unique_ptr<InstanceObject> boxObject;
	std::vector<std::vector<std::vector<BOX_INFO>>> boxInfo;

	//マウスのレイ情報
	Ray mouseLay;

	//現在の指定面
	XMINT4 pointface;
};

