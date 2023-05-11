#include "Map.h"
#include "Collision.h"
#include "DebugText.h"
#include <JsonLoader.h>
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

bool Map::ColRayBox(const XMFLOAT3& _point1, const XMFLOAT3& _point2, const XMMATRIX& _boxMat, HIT_INFO* _hitInfo) {
	Box box;
	box.point1 = _point1;
	box.point2 = _point2;

	HIT_INFO rNum;

	if (Collision::CheckRayAabb(mouseLay, box, _boxMat, &rNum.dist, &rNum.hitPos)) {
		if ((mouseLay.dir.m128_f32[0] < 0 && mouseLay.start.m128_f32[0] < rNum.hitPos.m128_f32[0]) ||
			(mouseLay.dir.m128_f32[0] > 0 && mouseLay.start.m128_f32[0] > rNum.hitPos.m128_f32[0]) ||
			(mouseLay.dir.m128_f32[1] < 0 && mouseLay.start.m128_f32[1] < rNum.hitPos.m128_f32[1]) ||
			(mouseLay.dir.m128_f32[1] > 0 && mouseLay.start.m128_f32[1] > rNum.hitPos.m128_f32[1]) ||
			(mouseLay.dir.m128_f32[2] < 0 && mouseLay.start.m128_f32[2] < rNum.hitPos.m128_f32[2]) ||
			(mouseLay.dir.m128_f32[2] > 0 && mouseLay.start.m128_f32[2] > rNum.hitPos.m128_f32[2]) || rNum.dist<10.0f) {
			if (_hitInfo) {
				_hitInfo->dist = 501.0f;
			}
			return false;
		}
		if (_hitInfo) {
			*_hitInfo = rNum;
		}
		return true;
	}

	if (_hitInfo) {
		_hitInfo->dist = 501.0f;
	}
	return false;
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
	const XMINT3 motoSize = delimitNum;
	delimitNum = _delimitNum;

	boxInfo.resize(delimitNum.y);

	for (int y = 0; y < delimitNum.y; y++) {
		boxInfo[y].resize(delimitNum.z);
		for (int z = 0; z < delimitNum.z; z++) {
			boxInfo[y][z].resize(delimitNum.x);
			for (int x = 0; x < delimitNum.x; x++) {
				if (x > motoSize.x || y > motoSize.y || z > motoSize.z) {
					boxInfo[y][z][x].type = TYPE::NONE;
				}
				boxInfo[y][z][x].pos = { delimitSize / 2.0f + x * delimitSize ,delimitSize / 2.0f + y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
			}
		}
	}

	CreateLine();
	ImportFace();
}

void Map::Update(const XMFLOAT3& _pos, const XMFLOAT3& _cameraTarget, const bool _kaburi, const bool _isOutsideCollision)
{
	//現在の配置場所
	{
		mouseLay.start = { _pos.x,_pos.y,_pos.z };
		mouseLay.dir = { _cameraTarget.x - _pos.x ,_cameraTarget.y - _pos.y,_cameraTarget.z - _pos.z };
		mouseLay.dir = DirectX::XMVector3Normalize(mouseLay.dir);

		//情報初期化
		hitInfo.dist = 501.0f;

		HIT_INFO hinfo;
		const float zure = delimitSize / 2.0f;
		//面更新
		int xx = 0, yy = 0, zz = 0;
		for (auto& y : faceInfo)
		{
			for (auto& z : y)
			{
				for (auto& x : z)
				{
					//xz平面
					if ((x.face[0].isPossibleHit && !x.face[0].isEdge) || (_isOutsideCollision && x.face[0].isEdge)) {
						//色初期化
						x.face[0].color = { 1,1,1,1 };
						//ワールド行列
						WorldMat(x.face[0].pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f },
							x.face[0].rota, x.face[0].worldMat);
						//判定
						if (ColRayBox({ x.face[0].pos.x - zure ,x.face[0].pos.y - 0.05f,x.face[0].pos.z - zure },
							{ x.face[0].pos.x + zure ,x.face[0].pos.y + 0.05f,x.face[0].pos.z + zure },
							x.face[0].worldMat, &hinfo)) {

							//近い方に更新
							if (hitInfo.dist > hinfo.dist) {
								hitInfo = hinfo;
								pointface = { xx,yy,zz,0 };
							}
						}
					}
					//xy平面
					if ((x.face[1].isPossibleHit && !x.face[1].isEdge) || (_isOutsideCollision && x.face[1].isEdge)) {
						//色初期化
						x.face[1].color = { 1,1,1,1 };
						//ワールド行列
						WorldMat(x.face[1].pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f },
							x.face[1].rota, x.face[1].worldMat);
						//判定
						if (ColRayBox({ x.face[1].pos.x - zure ,x.face[1].pos.y - zure,x.face[1].pos.z - 0.05f },
							{ x.face[1].pos.x + zure ,x.face[1].pos.y + zure,x.face[1].pos.z + 0.05f },
							x.face[1].worldMat, &hinfo)) {

							//近い方に更新
							if (hitInfo.dist > hinfo.dist) {
								hitInfo = hinfo;
								pointface = { xx,yy,zz,1 };
							}
						}
					}
					//yz平面
					if ((x.face[2].isPossibleHit && !x.face[2].isEdge) || (_isOutsideCollision && x.face[2].isEdge)) {
						//色初期化
						x.face[2].color = { 1,1,1,1 };
						//ワールド行列
						WorldMat(x.face[2].pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f },
							x.face[2].rota, x.face[2].worldMat);
						//判定
						if (ColRayBox({ x.face[2].pos.x - 0.05f ,x.face[2].pos.y - zure,x.face[2].pos.z - zure },
							{ x.face[2].pos.x + 0.05f ,x.face[2].pos.y + zure,x.face[2].pos.z + zure },
							x.face[2].worldMat, &hinfo)) {

							//近い方に更新
							if (hitInfo.dist > hinfo.dist) {
								hitInfo = hinfo;
								pointface = { xx,yy,zz,2 };
							}
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

		if (hitInfo.dist != 501.0f) {
			//設置されているブロックとの判定
			isHitBox = false;
			for (auto& y : boxInfo)
			{
				for (auto& z : y)
				{
					for (auto& x : z)
					{
						if (x.type == TYPE::NONE) { continue; }
						WorldMat(x.pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f }, { 0.0f,0.0f,0.0f }, x.worldMat);
						//判定
						if (!ColRayBox({ x.pos.x - zure ,x.pos.y - zure,x.pos.z - zure },
							{ x.pos.x + zure ,x.pos.y + zure,x.pos.z + zure }, x.worldMat, &hinfo)) {
							continue;
						}

						if (!(hitInfo.dist > hinfo.dist)) { continue; }
						isHitBox = true;
						break;
					}
				}
			}
		} else {
			isHitBox = true;
		}
	}

	if (!isHitBox && !_kaburi) {
		faceObject->DrawInstance(faceInfo[pointface.y][pointface.z][pointface.x].face[pointface.w].worldMat, COLOR[1]);
	}
	
	//ボックス更新
	for (auto& y : boxInfo)
	{
		for (auto& z : y)
		{
			for (auto& x : z)
			{
				if (x.type == TYPE::PLAYER) {
					boxObject->DrawInstance(x.worldMat, { 0.2f,0.2f,0.8f,1.0f });
				}
				else if (x.type == TYPE::GOAL) {
					boxObject->DrawInstance(x.worldMat, { 0.8f,0.2f,0.2f,1.0f });
				}
				else if (x.type == TYPE::NORMAL) {
					boxObject->DrawInstance(x.worldMat, { 0.2f,0.8f,0.2f,1.0f });
				}
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
	if (faceObject->GetInstanceDrawNum() != 0) {
		faceObject->Draw();
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
			boxInfo[y][z].resize(_size.x);
			for (int x = 0; x < _size.x; x++) {
				boxInfo[y][z][x].type = TYPE(_map[y][z][x]);
				boxInfo[y][z][x].pos = { delimitSize / 2.0f + x * delimitSize ,delimitSize / 2.0f + y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
			}
		}
	}
}

void Map::AddBox(const XMFLOAT3& _cameraPos, const std::array<bool, 3> _isSetObject)
{
	//設置されたボックスと当たっている時は新しいボックスを設置しない
	if (isHitBox) {
		return;
	}
	//カメラと設置面との位置関係
	XMFLOAT3 zure = {};
	zure.x = hitInfo.hitPos.m128_f32[0] - _cameraPos.x;
	zure.y = hitInfo.hitPos.m128_f32[1] - _cameraPos.y;
	zure.z = hitInfo.hitPos.m128_f32[2] - _cameraPos.z;

	//設置オブジェクト
	TYPE setType = TYPE::NORMAL;
	for (int i = 0; i < TYPE::SIZE - 1; i++) {
		if (_isSetObject[i]) {
			setType = TYPE(i + 1);
			break;
		}
	}

	//xz平面の時
	if (pointface.w == 0) {
		//下から
		if (pointface.y == 0 || (zure.y < 0 && pointface.y != delimitNum.y)) {
			if (boxInfo[pointface.y][pointface.z][pointface.x].type != TYPE::NONE) {
				return;
			}
			//設置
			boxInfo[pointface.y][pointface.z][pointface.x].type = setType;
			//下面
			if (pointface.y != 0) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			}
			//前面
			if (pointface.z != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = true;
				}
			}
			//左面
			if (pointface.x != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = true;
				}
			}
			//右面
			if (pointface.x != delimitNum.x - 1){
				if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
				}
			}
			//奥面
			if (pointface.z != delimitNum.z - 1) {
				if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
				}
			}
			//上面
			if (pointface.y != delimitNum.y - 1) {
				if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
				}
			}
		}
		//上から
		else if (pointface.y == delimitNum.y || zure.y > 0) {
			if (boxInfo[pointface.y - 1][pointface.z][pointface.x].type != TYPE::NONE) {
				return;
			}
			//設置
			boxInfo[pointface.y - 1][pointface.z][pointface.x].type = setType;
			//上面
			if (pointface.y != delimitNum.y) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			}
			//下面
			if (pointface.y != 0) {
				if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
				}
			}
			//前面
			if (pointface.z != 0) {
				if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit = true;;
				}
			}
			//左面
			if (pointface.x != 0) {
				if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit) {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit = true;
				}
			}
			//右面
			if (pointface.x != delimitNum.x - 1) {
				if (faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
					faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
				}
			}
			//奥面
			if (pointface.z != delimitNum.z - 1) {
				if (faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
				}
			}
		}
		return;
	}
	//xy平面の時
	if (pointface.w == 1) {
		//前から
		if (pointface.z == 0 || (zure.z < 0 && pointface.z != delimitNum.z)) {
			if (boxInfo[pointface.y][pointface.z][pointface.x].type != TYPE::NONE) {
				return;
			}
			//設置
			boxInfo[pointface.y][pointface.z][pointface.x].type = setType;
			//前面
			if (pointface.z != 0) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			}
			//下面
			if (pointface.y != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = true;
				}
			}
			//左面
			if (pointface.x != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = true;
				}
			}
			//右面
			if (pointface.x != delimitNum.x - 1) {
				if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
				}
			}
			//奥面
			if (pointface.z != delimitNum.z - 1) {
				if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
				}
			}
			//上面
			if (pointface.y != delimitNum.y - 1) {
				if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
				}
			}
		}
		//奥から
		else if (pointface.z == delimitNum.z || zure.z > 0) {
			if (boxInfo[pointface.y][pointface.z - 1][pointface.x].type != TYPE::NONE) {
				return;
			}
			//設置
			boxInfo[pointface.y][pointface.z - 1][pointface.x].type = setType;
			//奥面
			if (pointface.z != delimitNum.z) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			}
			//下面
			if (pointface.y != 0) {
				if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit = true;
				}
			}
			//前面
			if (pointface.z != 0) {
				if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit = true;;
				}
			}
			//左面
			if (pointface.x != 0) {
				if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit = true;
				}
			}
			//上面
			if (pointface.y != delimitNum.y - 1) {
				if (faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit = true;
				}
			}
			//右面
			if (pointface.x != delimitNum.x - 1) {
				if (faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit = true;
				}
			}
		}
		return;
	}
	//yz平面の時
	if (pointface.w == 2) {
		//左から
		if (pointface.x == 0 || (zure.x < 0 && pointface.x != delimitNum.x)) {
			if (boxInfo[pointface.y][pointface.z][pointface.x].type != TYPE::NONE) {
				return;
			}
			//設置
			boxInfo[pointface.y][pointface.z][pointface.x].type = setType;
			//左面
			if (pointface.x != 0) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			}
			//下面
			if (pointface.y != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = true;
				}
			}
			//前面
			if (pointface.z != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = true;
				}
			}
			//右面
			if (pointface.x != delimitNum.x - 1) {
				if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
				}
			}
			//奥面
			if (pointface.z != delimitNum.z - 1) {
				if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
				}
			}
			//上面
			if(pointface.y != delimitNum.y-1)
			if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}

		}
		//右から
		else if (pointface.x == delimitNum.x || zure.x > 0) {
			if (boxInfo[pointface.y][pointface.z][pointface.x - 1].type != TYPE::NONE) {
				return;
			}
			//設置
			boxInfo[pointface.y][pointface.z][pointface.x - 1].type = setType;
			//右面
			if (pointface.x != delimitNum.x) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			}
			//下面
			if (pointface.y != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit = true;
				}
			}
			//前面
			if (pointface.z != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit = true;;
				}
			}
			//左面
			if (pointface.x != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit = true;
				}
			}
			//上面
			if (pointface.y != delimitNum.y - 1) {
				if (faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit) {
					faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit = true;
				}
			}
			//奥面
			if (pointface.z != delimitNum.z - 1) {
				if (faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit = true;
				}
			}
		}
		return;
	}
}

void Map::DeleteBox(const XMFLOAT3& _cameraPos)
{
	//カメラと設置面との位置関係
	XMFLOAT3 zure = {};
	zure.x = hitInfo.hitPos.m128_f32[0] - _cameraPos.x;
	zure.y = hitInfo.hitPos.m128_f32[1] - _cameraPos.y;
	zure.z = hitInfo.hitPos.m128_f32[2] - _cameraPos.z;

	//xz平面の時
	if (pointface.w == 0) {
		//上から
		if (pointface.y != 0 && zure.y < 0) {
			if (boxInfo[pointface.y - 1][pointface.z][pointface.x].type == TYPE::NONE) {
				return;
			}
			//削除
			boxInfo[pointface.y - 1][pointface.z][pointface.x].type = TYPE::NONE;
			//上面
			if (pointface.y != delimitNum.y) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			}
			//下面
			if (pointface.y != 0) {
				if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
				}
			}
			//前面
			if (pointface.z != 0) {
				if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit = true;
				}
			}
			//左面
			if (pointface.x != 0) {
				if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit) {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit = true;
				}
			}
			//右面
			if (pointface.x == delimitNum.x - 1) {
				if (faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
					faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
				}
			}
			//奥面
			if (pointface.z == delimitNum.z - 1) {
				if (faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
				}
			}
		}
		//下から
		else if (pointface.y != delimitNum.y && zure.y > 0) {
			if (boxInfo[pointface.y][pointface.z][pointface.x].type == TYPE::NONE) {
				return;
			}
			//削除
			boxInfo[pointface.y][pointface.z][pointface.x].type = TYPE::NONE;
			//下面
			if (pointface.y != 0) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			}
			//前面
			if (pointface.z != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = true;
				}
			}
			//左面
			if (pointface.x != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = true;
				}
			}
			//上面
			if (pointface.y != delimitNum.y - 1) {
				if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
				}
			}
			//右面
			if (pointface.x != delimitNum.x - 1) {
				if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
				}
			}
			//
			if (pointface.z != delimitNum.z - 1) {
				if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
				}
			}
		}
		return;
	}
	//xy平面の時
	if (pointface.w == 1) {
		//奥から
		if (pointface.z != 0 && zure.z < 0) {
			if (boxInfo[pointface.y][pointface.z - 1][pointface.x].type == TYPE::NONE) {
				return;
			}
			//削除
			boxInfo[pointface.y][pointface.z - 1][pointface.x].type = TYPE::NONE;
			//奥面
			if (pointface.z != delimitNum.z) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			}
			//下面
			if (pointface.y != 0) {
				if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit = true;
				}
			}
			//前面
			if (pointface.z != 0) {
				if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit = true;;
				}
			}
			//左面
			if (pointface.x != 0) {
				if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit = true;
				}
			}
			//上面
			if (pointface.y != delimitNum.y - 1) {
				if (faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit = true;
				}
			}
			//右面
			if (pointface.x != delimitNum.x - 1) {
				if (faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit = true;
				}
			}
		}
		//前から
		else if (pointface.z != delimitNum.z && zure.z > 0) {
			if (boxInfo[pointface.y][pointface.z][pointface.x].type == TYPE::NONE) {
				return;
			}
			//削除
			boxInfo[pointface.y][pointface.z][pointface.x].type = TYPE::NONE;
			//前面
			if (pointface.z != 0) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			}
			//下面
			if (pointface.y != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = true;
				}
			}
			//左面
			if (pointface.x != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = true;
				}
			}
			//右面
			if (pointface.x != delimitNum.x - 1) {
				if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
				}
			}
			//奥面
			if (pointface.z != delimitNum.z - 1) {
				if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
				}
			}
			//上面
			if (pointface.y != delimitNum.y - 1) {
				if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
				}
			}
		}
		return;
	}
	//yz平面の時
	if (pointface.w == 2) {
		//右から
		if (pointface.x != 0 && zure.x < 0) {
			if (boxInfo[pointface.y][pointface.z][pointface.x - 1].type == TYPE::NONE) {
				return;
			}
			//設置
			boxInfo[pointface.y][pointface.z][pointface.x - 1].type = TYPE::NONE;
			//右面
			if (pointface.x != delimitNum.x) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			}
			//下面
			if(pointface.y != 0){
			if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit = true;
			}
			}
			//前面
			if (pointface.z != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit = true;;
				}
			}
			//左面
			if (pointface.x != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit = true;
				}
			}
			//上面
			if (pointface.y != delimitNum.y - 1) {
				if (faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit) {
					faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit = true;
				}
			}
			//奥面
			if (pointface.z != delimitNum.z - 1) {
				if (faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit = true;
				}
			}
		}
		//左から
		else if (pointface.x != delimitNum.x && zure.x > 0) {
			if (boxInfo[pointface.y][pointface.z][pointface.x].type == TYPE::NONE) {
				return;
			}
			//削除
			boxInfo[pointface.y][pointface.z][pointface.x].type = TYPE::NONE;
			//左面
			if (pointface.x != 0) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			}
			//下面
			if (pointface.y != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = true;
				}
			}
			//前面
			if (pointface.z != 0) {
				if (faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = true;
				}
			}
			//右面
			if (pointface.x != delimitNum.x - 1) {
				if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
				}
			}
			//奥面
			if (pointface.z != delimitNum.z - 1) {
				if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
				} else {
					faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
				}
			}
			//上面
			if (pointface.y != delimitNum.y - 1) {
				if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
					faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
				} else {
					faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
				}
			}
		}
		return;
	}
}

void Map::OutputMap(const std::string& _fileName, float _cameraDist)
{
	std::vector<std::vector<std::vector<int>>> outmap;

	outmap.resize(delimitNum.y);
	for (int y = 0; y < delimitNum.y; y++) {
		outmap[y].resize(delimitNum.z);
		for (int z = 0; z < delimitNum.z; z++) {
			outmap[y][z].resize(delimitNum.x);
			for (int x = 0; x < delimitNum.x; x++) {
				outmap[y][z][x]= boxInfo[y][z][x].type;
			}
		}
	}

	JsonLoader::SerializeTest(_fileName + ".json", _cameraDist, { delimitNum.x,delimitNum.y ,delimitNum.z }, outmap);
}

bool Map::ImputMap(const std::string& _fileName)
{
	XMINT3 mapSize={};
	std::vector<std::vector<std::vector<int>>> imputmap;
	float _cameraDist;

	if (!JsonLoader::DeserializeTest(_fileName + ".json", &_cameraDist, &imputmap)) {
		return false;
	}

	delimitNum = { int(imputmap[0][0].size()),int(imputmap.size()),int(imputmap[0].size()) };

	boxInfo.resize(delimitNum.y);
	for (int y = 0; y < delimitNum.y; y++) {
		boxInfo[y].resize(delimitNum.z);
		for (int z = 0; z < delimitNum.z; z++) {
			boxInfo[y][z].resize(delimitNum.x);
			for (int x = 0; x < delimitNum.x; x++) {
				boxInfo[y][z][x].type = TYPE(imputmap[y][z][x]);
				boxInfo[y][z][x].pos = { delimitSize / 2.0f + x * delimitSize ,delimitSize / 2.0f + y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
			}
		}
	}

	CreateLine();
	ImportFace();

	return true;
}

void Map::Initialize(const float _delimitSize, const XMINT3& _delimitNum)
{
	//区切りサイズ
	delimitSize = _delimitSize;
	//区切り個数
	delimitNum = _delimitNum;

	faceModel = Model::CreateFromOBJ("Face");

	faceObject = InstanceObject::Create(faceModel.get());
	faceObject->SetBloom(false);
	faceObject->SetLight(false);

	//線と接地面の生成
	CreateLine();
	CreateFace();

	//ボックス生成
	boxInfo.clear();
	boxInfo.resize(delimitNum.y);

	for (int y = 0; y < delimitNum.y; y++) {
		boxInfo[y].resize(delimitNum.z);
		for (int z = 0; z < delimitNum.z; z++) {
			boxInfo[y][z].resize(delimitNum.x);
			for (int x = 0; x < delimitNum.x; x++) {
				boxInfo[y][z][x].type = TYPE::NONE;
				boxInfo[y][z][x].pos = { delimitSize / 2.0f + x * delimitSize ,delimitSize / 2.0f + y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
			}
		}
	}

	//ボックス初期化
	boxModel = Model::CreateFromOBJ("NormalCube");
	boxObject = InstanceObject::Create(boxModel.get());
	boxObject->SetBloom(false);
	boxObject->SetLight(true);

	pointface = { 0,0,0,0 };
}

void Map::CreateLine()
{
	lineObject.reset();
	lineObject = PrimitiveObject3D::Create();
	isChange = false;

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

	for (int x = 0; x < delimitNum.x; x++) {
		for (int y = 0; y < lineNumy; y++) {
			for (int z = 0; z < lineNumz; z++) {
				lineObject->SetVertex({ linePosList_x[x],linePosList_y[y],linePosList_z[z] });
				lineObject->SetVertex({ linePosList_x[x + 1],linePosList_y[y],linePosList_z[z] });
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

	lineObject->VertexInit();

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
					one_chip.face[0].isPossibleHit = true;
					one_chip.face[0].isEdge = true;
				}
				one_chip.face[0].pos = { delimitSize / 2.0f + x * delimitSize,
					y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
				one_chip.face[0].rota = { 0.0f ,0.0f,0.0f };

				//xy
				if ((z == 0 || z == delimitNum.z) && x != delimitNum.x && y != delimitNum.y) {
					one_chip.face[1].isPossibleHit = true;
					one_chip.face[1].isEdge = true;
				}
				one_chip.face[1].pos = { delimitSize / 2.0f + x * delimitSize,
					delimitSize / 2.0f + y * delimitSize ,z * delimitSize };
				one_chip.face[1].rota = { 90.0f ,0.0f,0.0f };

				//yz
				if ((x == 0 || x == delimitNum.x) && y != delimitNum.y && z != delimitNum.z) {
					one_chip.face[2].isPossibleHit = true;
					one_chip.face[2].isEdge = true;
				}
				one_chip.face[2].pos = { x * delimitSize,
					delimitSize / 2.0f + y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
				one_chip.face[2].rota = { 90.0f ,0.0f,90.0f };
			}
		}
	}
}

void Map::ImportFace()
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
				one_chip.face[0].pos = { delimitSize / 2.0f + x * delimitSize,
					y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
				one_chip.face[0].rota = { 0.0f ,0.0f,0.0f };

				//xy
				one_chip.face[1].pos = { delimitSize / 2.0f + x * delimitSize,
					delimitSize / 2.0f + y * delimitSize ,z * delimitSize };
				one_chip.face[1].rota = { 90.0f ,0.0f,0.0f };

				//yz
				one_chip.face[2].pos = { x * delimitSize,
					delimitSize / 2.0f + y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
				one_chip.face[2].rota = { 90.0f ,0.0f,90.0f };
			}
		}
	}

	for (int y = 0; y < f_y; y++) {
		faceInfo[y].resize(f_z);
		for (int z = 0; z < f_z; z++) {
			faceInfo[y][z].resize(f_x);
			for (int x = 0; x < f_x; x++) {
				FACE_CHIP& one_chip = faceInfo[y][z][x];

				if (x != delimitNum.x && y != delimitNum.y && z != delimitNum.z) {
					if (boxInfo[y][z][x].type != TYPE::NONE) {
						one_chip.face[0].isPossibleHit = !one_chip.face[0].isPossibleHit;
						one_chip.face[1].isPossibleHit = !one_chip.face[1].isPossibleHit;
						one_chip.face[2].isPossibleHit = !one_chip.face[2].isPossibleHit;
						faceInfo[y + 1][z][x].face[0].isPossibleHit = !faceInfo[y + 1][z][x].face[0].isPossibleHit;
						faceInfo[y][z + 1][x].face[1].isPossibleHit = !faceInfo[y][z + 1][x].face[1].isPossibleHit;
						faceInfo[y][z][x + 1].face[2].isPossibleHit = !faceInfo[y][z][x + 1].face[2].isPossibleHit;
					}
				}
				//xz
				if ((y == 0 || y == delimitNum.y) && x != delimitNum.x && z != delimitNum.z) {
					one_chip.face[0].isPossibleHit = true;
					one_chip.face[0].isEdge = true;
				}
				//xy
				if ((z == 0 || z == delimitNum.z) && x != delimitNum.x && y != delimitNum.y) {
					one_chip.face[1].isPossibleHit = true;
					one_chip.face[1].isEdge = true;
				}
				//yz
				if ((x == 0 || x == delimitNum.x) && y != delimitNum.y && z != delimitNum.z) {
					one_chip.face[2].isPossibleHit = true;
					one_chip.face[2].isEdge = true;
				}
			}
		}
	}
}