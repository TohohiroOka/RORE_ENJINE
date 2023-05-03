#include "Map.h"
#include "Collision.h"
#include "DebugText.h"
#include <JsonLoder.h>
#include "DirectInput.h"

const std::array<DirectX::XMFLOAT4,2> COLOR = {
DirectX::XMFLOAT4{ 0.5f,0.5f,0.5f,0.5f },DirectX::XMFLOAT4{1.0f,1.0f,1.0f,1.0f}};

void Map::WorldMat(const XMFLOAT3& _pos, const XMFLOAT3& _scale, const XMFLOAT3& _rotation, XMMATRIX& _worldMat) {
	//ワールド行列変換
	_worldMat = DirectX::XMMatrixIdentity();
	XMMATRIX matScale = DirectX::XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	_worldMat *= matScale;

	XMMATRIX matRot;//角度
	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(_rotation.z));
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(_rotation.x));
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(_rotation.y));
	_worldMat *= matRot;

	XMMATRIX matTrans;//平行方向
	matTrans = DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
	_worldMat *= matTrans;
}

float Map::ColRayBox(const XMFLOAT3& _point1, const XMFLOAT3& _point2,const XMMATRIX& _boxMat) {
	Box box;
	box.point1 = _point1;
	box.point2 = _point2;

	float t;

	if (Collision::CheckRayRangePlane(mouseLay, box, _boxMat, &t, nullptr)) {
		return t;
	}

	return 501.0f;
}

std::unique_ptr<Map> Map::Create(const float _delimitSize, const XMINT3& _delimitNum)
{
	// 3Dオブジェクトのインスタンスを生成
	Map* instance = new Map();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->Initialize(_delimitSize, _delimitNum);

	return std::unique_ptr<Map>(instance);
}

void Map::ChangeDelimitNum(const XMINT3& _delimitNum)
{
	delimitNum = _delimitNum;
	CreateLine();
	CreateFace();
}

void Map::Update(const XMFLOAT3& _pos, const XMFLOAT3& cameraTarget)
{
	{
		mouseLay.start = { _pos.x,_pos.y,_pos.z };
		mouseLay.dir = { cameraTarget.x - _pos.x,cameraTarget.y - _pos.y,cameraTarget.z - _pos.y };
		mouseLay.dir = DirectX::XMVector3Normalize(mouseLay.dir);

		float hitDist = 500.0f;
		const float zure = delimitSize / 2.0f;
		//面更新
		int xx = 0, yy = 0, zz = 0;
		for (auto& y : faceInfo)
		{
			for (auto& z : y)
			{
				for (auto& x : z)
				{
					float hitDistNow = 0;
					//xz平面
					if (x.face[0].isDraw) {
						//色初期化
						x.face[0].color={1,1,1,1};
						//ワールド行列
						WorldMat(x.face[0].pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f },
							x.face[0].rota, x.face[0].worldMat);
						//判定
						hitDistNow = ColRayBox({ x.face[0].pos.x - zure ,x.face[0].pos.y,x.face[0].pos.z - zure },
							{ x.face[0].pos.x + zure ,x.face[0].pos.y,x.face[0].pos.z + zure },x.face[0].worldMat);

						//近い方に更新
						if (hitDist > hitDistNow) {
							hitDist = hitDistNow;
							pointface = { xx,yy,zz,0 };
						}
					}
					//xy平面
					if (x.face[1].isDraw) {
						//色初期化
						x.face[1].color = { 1,1,1,1 };
						//ワールド行列
						WorldMat(x.face[1].pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f },
							x.face[1].rota, x.face[1].worldMat);
						//判定
						hitDistNow = ColRayBox({ x.face[1].pos.x - zure ,x.face[1].pos.y,x.face[1].pos.z - zure },
							{ x.face[1].pos.x + zure ,x.face[1].pos.y,x.face[0].pos.z + zure },x.face[1].worldMat);

						//近い方に更新
						if (hitDist > hitDistNow) {
							hitDist = hitDistNow;
							pointface = { xx,yy,zz,1 };
						}
					}
					//yz平面
					if (x.face[2].isDraw) {
						//色初期化
						x.face[2].color = { 1,1,1,1 };
						//ワールド行列
						WorldMat(x.face[2].pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f },
							x.face[2].rota, x.face[2].worldMat);
						//判定
						hitDistNow = ColRayBox({ x.face[2].pos.x - zure ,x.face[2].pos.y,x.face[2].pos.z - zure },
							{ x.face[2].pos.x + zure ,x.face[2].pos.y,x.face[2].pos.z + zure },x.face[2].worldMat);

						//近い方に更新
						if (hitDist > hitDistNow) {
							hitDist = hitDistNow;
							pointface = { xx,yy,zz,2 };
						}
					}
					xx++;
				}
				xx = 0;
				zz++;
			}
			zz = 0;
			yy++;
		}

		DebugText* text = DebugText::GetInstance();
		text->Print("hitNum x y z w : " + std::to_string(pointface.x) + ":" + std::to_string(pointface.y) + ":"
			+ std::to_string(pointface.z) + ":" + std::to_string(pointface.w), 100, 425);
		text = nullptr;
	}

	int xx = -1, yy = -1, zz = -1;
	//面描画指定
	for (auto& y : faceInfo)
	{
		yy++;
		for (auto& z : y)
		{
			zz++;
			for (auto& x : z)
			{
				xx++;
				for (auto& itr : faceObject)
				{
					if (!itr->GetInstanceDrawCheck()) { continue; }
					//xz平面
					if (x.face[0].isDraw) {
						if (pointface.x == xx && pointface.y == yy && pointface.z == zz && pointface.w == 0) {
							itr->DrawInstance(x.face[0].worldMat, COLOR[1]);
						} else {
							itr->DrawInstance(x.face[0].worldMat, COLOR[0]);
						}
					}
					//xy平面
					if (x.face[1].isDraw) {
						if (pointface.x == xx && pointface.y == yy && pointface.z == zz && pointface.w == 1) {
							itr->DrawInstance(x.face[1].worldMat, COLOR[1]);
						} else {
							itr->DrawInstance(x.face[1].worldMat, COLOR[0]);
						}
					}
					//yz平面
					if (x.face[2].isDraw) {
						if (pointface.x == xx && pointface.y == yy && pointface.z == zz && pointface.w == 2) {
							itr->DrawInstance(x.face[2].worldMat, COLOR[1]);
						} else {
							itr->DrawInstance(x.face[2].worldMat, COLOR[0]);
						}
					}
					break;
				}
			}
			xx = -1;
		}
		zz = -1;
	}

	//ボックス更新
	for (auto& y : boxInfo)
	{
		for (auto& z : y)
		{
			for (auto& x : z)
			{
				if (x.type != TYPE::NORMAL) { continue; }
				boxObject->DrawInstance(x.pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f });
			}
		}
	}
}

void Map::Draw()
{
	lineObject->Draw();
}

void Map::InstanceDraw()
{
	//面描画
	for (auto& itr : faceObject) {
		if (itr->GetInstanceDrawNum() == 0) { continue; }
		itr->Draw();
	}

	//ボックス描画
	boxObject->Draw();
}

void Map::LoadMap(const std::string _fileName)
{
	std::vector<std::vector<std::vector<int>>> _map = {
	{
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1}
	},
	{
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1}
	},
	{
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1}
	},
	{
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1}
	},
	{
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1},
	{1,1,1,1,1}
	} };
	XMINT3 _size = { 5,5,5 };

	//線と面生成
	ChangeDelimitNum(_size);

	//ボックス生成
	boxInfo.clear();
	boxInfo.resize(_size.y);

	for (int y = 0; y < _size.y; y++) {
		boxInfo[y].resize(_size.z);
		for (int z = 0; z < _size.z; z++) {
			boxInfo[y][z].resize(_size.y);
			for (int x = 0; x < _size.y; x++) {
				boxInfo[y][z][x].type = TYPE(_map[y][z][x]);
				boxInfo[y][z][x].pos = { delimitSize / 2.0f + x * delimitSize ,delimitSize / 2.0f + y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
			}
		}
	}
}

void Map::AddBox()
{
	//if (boxInfo[mapNum.y][mapNum.z][mapNum.x].type == TYPE::NONE) { return; }
	//boxInfo[mapNum.y][mapNum.z][mapNum.x].type = TYPE::NORMAL;

	//if (mapNum.y != 0 && boxInfo[mapNum.y - 1][mapNum.z][mapNum.x].type == TYPE::NONE) {
	//	
	//} else {

	//}

}

void Map::OutputMap(const std::string& _fileName, float _cameraDist)
{
	std::vector<std::vector<std::vector<int>>> outmap;

	outmap.resize(delimitNum.y);
	for (int y = 0; y < delimitNum.y; y++) {
		outmap[y].resize(delimitNum.z);
		for (int z = 0; z < delimitNum.z; z++) {
			outmap[y][z].resize(delimitNum.y);
			for (int x = 0; x < delimitNum.y; x++) {
				outmap[y][z][x]= boxInfo[y][z][x].type;
			}
		}
	}

	JsonLoder::SerializeTest(_fileName + ".json", _cameraDist, { delimitNum.x,delimitNum.y ,delimitNum.z }, outmap);
}

void Map::Initialize(const float _delimitSize, const XMINT3& _delimitNum)
{
	//区切りサイズ
	delimitSize = _delimitSize;
	//区切り個数
	delimitNum = _delimitNum;

	lineObject = std::make_unique<PrimitiveObject3D>();
	lineObject->UpdateWorldMatrix();
	isChange = false;

	faceModel = Model::CreateFromOBJ("Face");

	for (auto& itr : faceObject) {
		itr = InstanceObject::Create(faceModel.get());
		itr->SetBloom(false);
		itr->SetLight(false);
	}
	//線と接地面の生成
	CreateLine();
	CreateFace();

	//ボックス初期化
	boxModel = Model::CreateFromOBJ("NormalCube");
	boxObject = InstanceObject::Create(boxModel.get());
	boxObject->SetBloom(false);
	boxObject->SetLight(false);

	pointface = { 0,0,0,0 };
}

void Map::CreateLine()
{
	//線配列初期化
	lineObject->ResetVertex();

	//線の数
	const int lineNumx = delimitNum.x + 1;
	const int lineNumy = delimitNum.y + 1;
	const int lineNumz = delimitNum.z + 1;

	//配列のサイズ変更
	linePosList_x.clear();
	linePosList_y.clear();
	linePosList_z.clear();
	linePosList_x.resize(lineNumx);
	linePosList_y.resize(lineNumy);
	linePosList_z.resize(lineNumz);

	//x
	for (int i = 0; i < lineNumx; i++) {
		linePosList_x[i] = float(delimitSize * i);
	}
	//y
	for (int i = 0; i < lineNumy; i++) {
		linePosList_y[i] = float(delimitSize * i);
	}
	//z
	for (int i = 0; i < lineNumz; i++) {
		linePosList_z[i] = float(delimitSize * i);
	}

	int a=0;
	for (int x = 0; x < delimitNum.x; x++) {
		for (int y = 0; y < lineNumy; y++) {
			for (int z = 0; z < lineNumz; z++) {
				lineObject->SetVertex({ linePosList_x[x],linePosList_y[y],linePosList_z[z] });
				lineObject->SetVertex({ linePosList_x[x + 1],linePosList_y[y],linePosList_z[z] });
				a++;
			}
		}
	}
	for (int y = 0; y < delimitNum.y; y++) {
		for (int x = 0; x < lineNumx; x++) {
			for (int z = 0; z < lineNumz; z++) {
				lineObject->SetVertex({ linePosList_x[x],linePosList_y[y],linePosList_z[z] });
				lineObject->SetVertex({ linePosList_x[x],linePosList_y[y + 1],linePosList_z[z] });
			}
		}
	}
	for (int z = 0; z < delimitNum.z; z++) {
		for (int x = 0; x < lineNumx; x++) {
			for (int y = 0; y < lineNumy; y++) {
				lineObject->SetVertex({ linePosList_x[x],linePosList_y[y],linePosList_z[z] });
				lineObject->SetVertex({ linePosList_x[x],linePosList_y[y],linePosList_z[z + 1] });
			}
		}
	}

	lineObject->Initialize();

	isChange = false;
}

void Map::CreateFace()
{
	faceInfo.clear();

	//頂点数
	const int f_x = delimitNum.x + 1;
	const int f_y = delimitNum.y + 1;
	const int f_z = delimitNum.z + 1;

	faceInfo.resize(f_y);

	for (int y = 0; y < f_y; y++) {
		faceInfo[y].resize(f_z);
		for (int z = 0; z < f_z; z++) {
			faceInfo[y][z].resize(f_x);
			for (int x = 0; x < f_x; x++) {
				FACE_CHIP& one_chip = faceInfo[y][z][x];
				//xz
				if ((y == 0 || y == delimitNum.y) && x != delimitNum.x && z != delimitNum.z) {
					one_chip.face[0].isDraw = true;
				}
				one_chip.face[0].pos = { delimitSize / 2.0f + x * delimitSize,
					y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
				one_chip.face[0].rota = { 0.0f ,0.0f,0.0f };

				//xy
				if ((z == 0 || z == delimitNum.z) && x != delimitNum.x && y != delimitNum.y) {
					one_chip.face[1].isDraw = true;
				}
				one_chip.face[1].pos = { delimitSize / 2.0f + x * delimitSize,
					delimitSize / 2.0f + y * delimitSize ,z * delimitSize };
				one_chip.face[1].rota = { 90.0f ,0.0f,0.0f };

				//yz
				if ((x == 0 || x == delimitNum.x) && y != delimitNum.y && z != delimitNum.z) {
					one_chip.face[2].isDraw = true;
				}
				one_chip.face[2].pos = { x * delimitSize,
					delimitSize / 2.0f + y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
				one_chip.face[2].rota = { 90.0f ,0.0f,90.0f };
			}
		}
	}
}

//void Map::Move(const float _angle)
//{
//	DirectInput* input = DirectInput::GetInstance();
//
//	enum key {
//		pz,
//		mz,
//		px,
//		mx
//	};
//
//	//+z/-z/+x/+y
//	std::array<bool, 4> key = { false,false,false,false };
//
//	//入力
//	{
//		//前
//		if (input->PushKey(DIK_W)) {
//			if (_angle > -45 && _angle < 45) {
//				key[mz] = true;
//			} else if((_angle >= 45 && _angle < 135)||(_angle >= -225 && _angle < -315)) {
//				key[px] = true;
//			} else if ((_angle >= 135 && _angle < 225) || (_angle >= -225 && _angle < -135)) {
//				key[pz] = true;
//			} else {
//				key[mx] = true;
//			}
//		}
//		//後
//		if (input->PushKey(DIK_S)) {
//			if (_angle > -45 && _angle < 45) {
//				key[pz] = true;
//			} else if ((_angle >= 45 && _angle < 135) || (_angle >= -225 && _angle < -315)) {
//				key[mx] = true;
//			} else if ((_angle >= 135 && _angle < 225) || (_angle >= -225 && _angle < -135)) {
//				key[mz] = true;
//			} else {
//				key[px] = true;
//			}
//		}
//		//右
//		if (input->PushKey(DIK_D)) {
//			if (_angle > -45 && _angle < 45) {
//				key[mx] = true;
//			} else if ((_angle >= 45 && _angle < 135) || (_angle >= -225 && _angle < -315)) {
//				key[pz] = true;
//			} else if ((_angle >= 135 && _angle < 225) || (_angle >= -225 && _angle < -135)) {
//				key[mx] = true;
//			} else {
//				key[mz] = true;
//			}
//		}
//		//左
//		if (input->PushKey(DIK_A)) {
//			if (_angle > -45 && _angle < 45) {
//				key[px] = true;
//			} else if ((_angle >= 45 && _angle < 135) || (_angle >= -225 && _angle < -315)) {
//				key[mz] = true;
//			} else if ((_angle >= 135 && _angle < 225) || (_angle >= -225 && _angle < -135)) {
//				key[px] = true;
//			} else {
//				key[pz] = true;
//			}
//		}
//	}
//
//	//enum F_point {
//	//	Px,
//	//	Py,
//	//	Pz,
//	//	Pw,
//	//	IN_OUT,
//	//	SIZE
//	//};
//
//	//+z方向
//	if (key[pz])
//	{
//		//xz平面にいる時
//		if (pointface.w == 0) {
//			//xz平面で奥に移動
//			//前下が接地面である、前方にオブジェクトが存在しない
//			if (pointface[Pz] + 1 < delimitNum.x && faceInfo[pointface[Py]][pointface[Pz] + 1][pointface[Px]].face[0].isDraw&&
//				((pointface[IN_OUT] == 0 && !faceInfo[pointface[Py]][pointface[Pz] + 1][pointface[Px]].face[1].isDraw)||
//					(pointface[IN_OUT] == 1 && !faceInfo[pointface[Py] - 1][pointface[Pz] + 1][pointface[Px]].face[1].isDraw))) {
//				pointface[Pz]++;
//			}
//			//xz平面からxy平面上に移動
//			//中は前方にオブジェクトが存在する、外は前に接地面が無い
//			else if (pointface[Pz] + 1 == delimitNum.x ||
//				((pointface[IN_OUT] == 0 && faceInfo[pointface[Py]][pointface[Pz] + 1][pointface[Px]].face[1].isDraw) ||
//					(pointface[IN_OUT] == 1 && !faceInfo[pointface[Py] - 1][pointface[Pz] + 1][pointface[Px]].face[1].isDraw))) {
//				pointface[Pz]++;
//				pointface[Pw] = 1;
//				//面移動の際に中外が変わる
//				if (pointface[IN_OUT] == 1) {
//					pointface[IN_OUT] = 0;
//				} else {
//					pointface[IN_OUT] = 1;
//				}
//			}
//			//xz平面からxy平面下に移動
//			else if (pointface[Pz] + 1 == delimitNum.x ||
//				((pointface[IN_OUT] == 0 && faceInfo[pointface[Py]][pointface[Pz] + 1][pointface[Px]].face[1].isDraw) ||
//					(pointface[IN_OUT] == 1 && !faceInfo[pointface[Py] - 1][pointface[Pz] + 1][pointface[Px]].face[1].isDraw))) {
//				pointface.z++;
//				pointface.y--;
//				pointface.w = 1;
//			}
//		}
//		//xy平面にいる時
//		else if (pointface.w == 1) {
//			//xy平面で上に移動
//			if (pointface.y + 1 < delimitNum.y && faceInfo[pointface.y + 1][pointface.z][pointface.x].face[1].isDraw) {
//				pointface.y++;
//			}
//			//xy平面からxz平面前に移動
//			else if (pointface.y + 1 == delimitNum.y || faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isDraw) {
//				pointface.y++;
//				pointface.z--;
//				pointface.w = 0;
//			}
//			//xy平面からxz平面奥に移動
//			else if (pointface.y + 1 <= delimitNum.y && !faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isDraw) {
//				pointface.y++;
//				pointface.w = 0;
//			}
//		}
//		//xy平面にいる時
//		else if (pointface.w == 2) {
//			//yz平面で奥に移動
//			if (pointface.z + 1 < delimitNum.z && !faceInfo[pointface.y][pointface.z+ 1][pointface.x].face[1].isDraw) {
//				pointface.y++;
//			}
//			//xy平面からxz平面前に移動
//			else if (pointface.y + 1 == delimitNum.y || faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isDraw) {
//				pointface.y++;
//				pointface.z--;
//				pointface.w = 0;
//			}
//			//xy平面からxz平面奥に移動
//			else if (pointface.y + 1 <= delimitNum.y && !faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isDraw) {
//				pointface.y++;
//				pointface.w = 0;
//			}
//		}
//
//	input = nullptr;
//}
//}
