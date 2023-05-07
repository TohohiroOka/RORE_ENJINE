#include "Map.h"
#include "Collision.h"
#include "DebugText.h"
#include <JsonLoder.h>
#include "DirectInput.h"

const std::array<DirectX::XMFLOAT4,2> COLOR = {
DirectX::XMFLOAT4{ 0.5f,0.5f,0.5f,0.5f },DirectX::XMFLOAT4{1.0f,1.0f,1.0f,1.0f}};

void Map::WorldMat(const XMFLOAT3& _pos, const XMFLOAT3& _scale, const XMFLOAT3& _rotation, XMMATRIX& _worldMat) {
	//���[���h�s��ϊ�
	_worldMat = DirectX::XMMatrixIdentity();
	XMMATRIX matScale = DirectX::XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	_worldMat *= matScale;

	XMMATRIX matRot;//�p�x
	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(_rotation.z));
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(_rotation.x));
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(_rotation.y));
	_worldMat *= matRot;

	XMMATRIX matTrans;//���s����
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
			(mouseLay.dir.m128_f32[2] > 0 && mouseLay.start.m128_f32[2] > rNum.hitPos.m128_f32[2])) {
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
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
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

void Map::Update(const XMFLOAT3& _pos, const XMFLOAT3& _cameraTarget, const bool _kaburi)
{
	//���݂̔z�u�ꏊ
	{
		mouseLay.start = { _pos.x,_pos.y,_pos.z };
		mouseLay.dir = { _cameraTarget.x - _pos.x ,_cameraTarget.y - _pos.y,_cameraTarget.z - _pos.z };
		mouseLay.dir = DirectX::XMVector3Normalize(mouseLay.dir);

		//��񏉊���
		hitInfo.dist = 501.0f;

		HIT_INFO hinfo;
		const float zure = delimitSize / 2.0f;
		//�ʍX�V
		int xx = 0, yy = 0, zz = 0;
		for (auto& y : faceInfo)
		{
			for (auto& z : y)
			{
				for (auto& x : z)
				{
					//xz����
					if (x.face[0].isPossibleHit) {
						//�F������
						x.face[0].color = { 1,1,1,1 };
						//���[���h�s��
						WorldMat(x.face[0].pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f },
							x.face[0].rota, x.face[0].worldMat);
						//����
						if (ColRayBox({ x.face[0].pos.x - zure ,x.face[0].pos.y - 0.05f,x.face[0].pos.z - zure },
							{ x.face[0].pos.x + zure ,x.face[0].pos.y + 0.05f,x.face[0].pos.z + zure },
							x.face[0].worldMat, &hinfo)) {

							//�߂����ɍX�V
							if (hitInfo.dist > hinfo.dist) {
								hitInfo = hinfo;
								pointface = { xx,yy,zz,0 };
							}
						}
					}
					//xy����
					if (x.face[1].isPossibleHit) {
						//�F������
						x.face[1].color = { 1,1,1,1 };
						//���[���h�s��
						WorldMat(x.face[1].pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f },
							x.face[1].rota, x.face[1].worldMat);
						//����
						if (ColRayBox({ x.face[1].pos.x - zure ,x.face[1].pos.y - zure,x.face[1].pos.z - 0.05f },
							{ x.face[1].pos.x + zure ,x.face[1].pos.y + zure,x.face[1].pos.z + 0.05f },
							x.face[1].worldMat, &hinfo)) {

							//�߂����ɍX�V
							if (hitInfo.dist > hinfo.dist) {
								hitInfo = hinfo;
								pointface = { xx,yy,zz,1 };
							}
						}
					}
					//yz����
					if (x.face[2].isPossibleHit) {
						//�F������
						x.face[2].color = { 1,1,1,1 };
						//���[���h�s��
						WorldMat(x.face[2].pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f },
							x.face[2].rota, x.face[2].worldMat);
						//����
						if (ColRayBox({ x.face[2].pos.x - 0.05f ,x.face[2].pos.y - zure,x.face[2].pos.z - zure },
							{ x.face[2].pos.x + 0.05f ,x.face[2].pos.y + zure,x.face[2].pos.z + zure },
							x.face[2].worldMat, &hinfo)) {

							//�߂����ɍX�V
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
			//�ݒu����Ă���u���b�N�Ƃ̔���
			isHitBox = false;
			for (auto& y : boxInfo)
			{
				for (auto& z : y)
				{
					for (auto& x : z)
					{
						if (x.type != TYPE::NORMAL) { continue; }
						WorldMat(x.pos, { delimitSize / 2.0f ,delimitSize / 2.0f ,delimitSize / 2.0f }, { 0.0f,0.0f,0.0f }, x.worldMat);
						//����
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
		//DebugText* text = DebugText::GetInstance();
		//text->Print("hitNum x y z w : " + std::to_string(pointface.x) + ":" + std::to_string(pointface.y) + ":"
		//	+ std::to_string(pointface.z) + ":" + std::to_string(pointface.w), 100, 425);
		//text = nullptr;
	}

	if (!isHitBox && !_kaburi) {
		faceObject->DrawInstance(faceInfo[pointface.y][pointface.z][pointface.x].face[pointface.w].worldMat, COLOR[1]);
	}
	
	//int xx = -1, yy = -1, zz = -1;
	////�ʕ`��w��
	//for (auto& y : faceInfo)
	//{
	//	yy++;
	//	for (auto& z : y)
	//	{
	//		zz++;
	//		for (auto& x : z)
	//		{
	//			xx++;
	//			for (auto& itr : faceObject)
	//			{
	//				if (!itr->GetInstanceDrawCheck()) { continue; }
	//				//xz����
	//				if (x.face[0].isPossibleHit) {
	//					if (pointface.x == xx && pointface.y == yy && pointface.z == zz && pointface.w == 0) {
	//						itr->DrawInstance(x.face[0].worldMat, COLOR[1]);
	//					} else {
	//						itr->DrawInstance(x.face[0].worldMat, COLOR[0]);
	//					}
	//				}
	//				//xy����
	//				if (x.face[1].isPossibleHit) {
	//					if (pointface.x == xx && pointface.y == yy && pointface.z == zz && pointface.w == 1) {
	//						itr->DrawInstance(x.face[1].worldMat, COLOR[1]);
	//					} else {
	//						itr->DrawInstance(x.face[1].worldMat, COLOR[0]);
	//					}
	//				}
	//				//yz����
	//				if (x.face[2].isPossibleHit) {
	//					if (pointface.x == xx && pointface.y == yy && pointface.z == zz && pointface.w == 2) {
	//						itr->DrawInstance(x.face[2].worldMat, COLOR[1]);
	//					} else {
	//						itr->DrawInstance(x.face[2].worldMat, COLOR[0]);
	//					}
	//				}
	//				break;
	//			}
	//		}
	//		xx = -1;
	//	}
	//	zz = -1;
	//}

	//�{�b�N�X�X�V
	for (auto& y : boxInfo)
	{
		for (auto& z : y)
		{
			for (auto& x : z)
			{
				if (x.type != TYPE::NORMAL) { continue; }
				boxObject->DrawInstance(x.worldMat, { 1.0f,1.0f,1.0f,1.0f });
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
	//�ʕ`��
	if (faceObject->GetInstanceDrawNum() != 0) {
		faceObject->Draw();
	}
	//�{�b�N�X�`��
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

	//���Ɩʐ���
	ChangeDelimitNum(_size);

	//�{�b�N�X����
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

void Map::AddBox(const XMFLOAT3& _cameraPos)
{
	//�ݒu���ꂽ�{�b�N�X�Ɠ������Ă��鎞�͐V�����{�b�N�X��ݒu���Ȃ�
	if (isHitBox) {
		return;
	}
	//�J�����Ɛݒu�ʂƂ̈ʒu�֌W
	XMFLOAT3 zure = {};
	zure.x = hitInfo.hitPos.m128_f32[0] - _cameraPos.x;
	zure.y = hitInfo.hitPos.m128_f32[1] - _cameraPos.y;
	zure.z = hitInfo.hitPos.m128_f32[2] - _cameraPos.z;

	//xz���ʂ̎�
	if (pointface.w == 0) {
		if (pointface.y == 0 || (zure.y < 0&& pointface.y != delimitNum.y)) {
			//�ݒu
			boxInfo[pointface.y][pointface.z][pointface.x].type = TYPE::NORMAL;
			//�ݒu�s��
			faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = true;;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
			}
			if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}
		} else if (pointface.y == delimitNum.y || zure.y > 0) {
			//�ݒu
			boxInfo[pointface.y - 1][pointface.z][pointface.x].type = TYPE::NORMAL;
			//�ݒu�s��
			faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit = true;;
			}
			if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit) {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
				faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
			}
		}
		return;
	}
	//xy���ʂ̎�
	if (pointface.w == 1) {
		if (pointface.z == 0 || (zure.z < 0 && pointface.z != delimitNum.z)) {
			//�ݒu
			boxInfo[pointface.y][pointface.z][pointface.x].type = TYPE::NORMAL;
			//�ݒu�s��
			faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
			}
			if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}
		} else if (pointface.z == delimitNum.z || zure.z > 0) {
			//�ݒu
			boxInfo[pointface.y][pointface.z - 1][pointface.x].type = TYPE::NORMAL;
			//�ݒu�s��
			faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit = true;;
			}
			if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit = true;
			}
		}
		return;
	}
	//yz���ʂ̎�
	if (pointface.w == 2) {
		if (pointface.x == 0 || (zure.x < 0 && pointface.x != delimitNum.x)) {
			//�ݒu
			boxInfo[pointface.y][pointface.z][pointface.x].type = TYPE::NORMAL;
			//�ݒu�s��
			faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
			}
			if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}

		} else if (pointface.x == delimitNum.x || zure.x > 0) {
			//�ݒu
			boxInfo[pointface.y][pointface.z][pointface.x - 1].type = TYPE::NORMAL;
			//�ݒu�s��
			faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit = true;;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit) {
				faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit = true;
			}
		}
		return;
	}
}

void Map::DeleteBox(const XMFLOAT3& _cameraPos)
{
	//�J�����Ɛݒu�ʂƂ̈ʒu�֌W
	XMFLOAT3 zure = {};
	zure.x = hitInfo.hitPos.m128_f32[0] - _cameraPos.x;
	zure.y = hitInfo.hitPos.m128_f32[1] - _cameraPos.y;
	zure.z = hitInfo.hitPos.m128_f32[2] - _cameraPos.z;

	//xz���ʂ̎�
	if (pointface.w == 0) {
		//�ォ��
		if (pointface.y != 0 && zure.y < 0 && boxInfo[pointface.y - 1][pointface.z][pointface.x].type == TYPE::NORMAL) {
			//�폜
			boxInfo[pointface.y - 1][pointface.z][pointface.x].type = TYPE::NONE;
			//�ݒu�s�ύX
			faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[1].isPossibleHit = true;
			}
			if (faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit) {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y - 1][pointface.z][pointface.x].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
				faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y - 1][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y - 1][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
			}
		} else if (pointface.y != delimitNum.y && zure.y > 0 && boxInfo[pointface.y][pointface.z][pointface.x].type == TYPE::NORMAL) {
			//�폜
			boxInfo[pointface.y][pointface.z][pointface.x].type = TYPE::NONE;
			//�ݒu�s�ύX
			faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
			}
		}
		return;
	}
	//xy���ʂ̎�
	if (pointface.w == 1) {
		if (pointface.z != 0 && zure.z < 0 && boxInfo[pointface.y][pointface.z - 1][pointface.x].type == TYPE::NORMAL) {
			//�폜
			boxInfo[pointface.y][pointface.z - 1][pointface.x].type = TYPE::NONE;
			//�ݒu�s��
			faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[1].isPossibleHit = true;;
			}
			if (faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z - 1][pointface.x].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y + 1][pointface.z - 1][pointface.x].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z - 1][pointface.x + 1].face[2].isPossibleHit = true;
			}
		} else if (pointface.z != delimitNum.z && zure.z > 0 && boxInfo[pointface.y][pointface.z][pointface.x].type == TYPE::NORMAL) {
			//�폜
			boxInfo[pointface.y][pointface.z][pointface.x].type = TYPE::NONE;
			//�ݒu�s��
			faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
			}
			if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}
		}
		return;
	}
	//yz���ʂ̎�
	if (pointface.w == 2) {
		if (pointface.x != 0 && zure.x < 0 && boxInfo[pointface.y][pointface.z][pointface.x - 1].type == TYPE::NORMAL) {
			//�ݒu
			boxInfo[pointface.y][pointface.z][pointface.x - 1].type = TYPE::NONE;
			//�ݒu�s��
			faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[1].isPossibleHit = true;;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x - 1].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit) {
				faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y + 1][pointface.z][pointface.x - 1].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z + 1][pointface.x - 1].face[1].isPossibleHit = true;
			}
		} else if (pointface.x != delimitNum.x && zure.x > 0 && boxInfo[pointface.y][pointface.z][pointface.x].type == TYPE::NORMAL) {
			//�폜
			boxInfo[pointface.y][pointface.z][pointface.x].type = TYPE::NONE;
			//�ݒu�s��
			faceInfo[pointface.y][pointface.z][pointface.x].face[2].isPossibleHit = false;
			//�s��ԂȂ�\�ɁA�\��ԂȂ�s��Ԃɂ���
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[0].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x].face[1].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit) {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z][pointface.x + 1].face[2].isPossibleHit = true;
			}
			if (faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit) {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = false;
			} else {
				faceInfo[pointface.y][pointface.z + 1][pointface.x].face[1].isPossibleHit = true;
			}
			if (faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit) {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = false;
			} else {
				faceInfo[pointface.y + 1][pointface.z][pointface.x].face[0].isPossibleHit = true;
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
	//��؂�T�C�Y
	delimitSize = _delimitSize;
	//��؂��
	delimitNum = _delimitNum;

	faceModel = Model::CreateFromOBJ("Face");

	faceObject = InstanceObject::Create(faceModel.get());
	faceObject->SetBloom(false);
	faceObject->SetLight(false);
	
	//���Ɛڒn�ʂ̐���
	CreateLine();
	CreateFace();

	//�{�b�N�X����
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

	//�{�b�N�X������
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

	//���̐�
	const int lineNumx = delimitNum.x + 1;
	const int lineNumy = delimitNum.y + 1;
	const int lineNumz = delimitNum.z + 1;

	//�z��̃T�C�Y�ύX
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

	lineObject->VertexInit();

	isChange = false;
}

void Map::CreateFace()
{
	faceInfo.clear();

	//���_��
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
				}
				one_chip.face[0].pos = { delimitSize / 2.0f + x * delimitSize,
					y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
				one_chip.face[0].rota = { 0.0f ,0.0f,0.0f };

				//xy
				if ((z == 0 || z == delimitNum.z) && x != delimitNum.x && y != delimitNum.y) {
					one_chip.face[1].isPossibleHit = true;
				}
				one_chip.face[1].pos = { delimitSize / 2.0f + x * delimitSize,
					delimitSize / 2.0f + y * delimitSize ,z * delimitSize };
				one_chip.face[1].rota = { 90.0f ,0.0f,0.0f };

				//yz
				if ((x == 0 || x == delimitNum.x) && y != delimitNum.y && z != delimitNum.z) {
					one_chip.face[2].isPossibleHit = true;
				}
				one_chip.face[2].pos = { x * delimitSize,
					delimitSize / 2.0f + y * delimitSize ,delimitSize / 2.0f + z * delimitSize };
				one_chip.face[2].rota = { 90.0f ,0.0f,90.0f };
			}
		}
	}
}