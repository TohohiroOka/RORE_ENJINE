#include "MeshCollider.h"
#include "Collision.h"
#include <bitset>

using namespace DirectX;

void MeshCollider::MinMax(Model* _model)
{
	const std::vector<Mesh*>& meshes = _model->GetMeshes();
	std::vector<Mesh*>::const_iterator it = meshes.cbegin();

	//[0]最大[1]最小
	XMFLOAT3 minmax[2] = { {0,0,0},{100,100,100} };

	for (; it != meshes.cend(); ++it) {
		Mesh* mesh = *it;
		const std::vector<Mesh::VERTEX>& vertices = mesh->GetVertices();
		const int size = static_cast<int>(vertices.size());

		for (int i = 0; i < size; i++)
		{
			if (vertices[i].pos.x > minmax[0].x)
			{
				minmax[0].x = vertices[i].pos.x;
			} else if (vertices[i].pos.x < minmax[1].x)
			{
				minmax[1].x = vertices[i].pos.x;
			}

			if (vertices[i].pos.y > minmax[0].y)
			{
				minmax[0].y = vertices[i].pos.y;
			} else if (vertices[i].pos.y < minmax[1].y)
			{
				minmax[1].y = vertices[i].pos.y;
			}

			if (vertices[i].pos.z > minmax[0].z)
			{
				minmax[0].z = vertices[i].pos.z;
			} else if (vertices[i].pos.z < minmax[1].z)
			{
				minmax[1].z = vertices[i].pos.z;
			}
		}
	}

	min = minmax[1];
	max = minmax[0];
}

void MeshCollider::MinMax(const std::vector<Mesh::VERTEX>* _vertices)
{
	//[0]最大[1]最小
	XMFLOAT3 minmax[2] = { {0,0,0},{100,100,100} };

	const std::vector<Mesh::VERTEX> vertices = *_vertices;
	const int size = static_cast<int>(vertices.size());

	for (int i = 0; i < size; i++)
	{
		if (vertices[i].pos.x > minmax[0].x)
		{
			minmax[0].x = vertices[i].pos.x;
		} else if (vertices[i].pos.x < minmax[1].x)
		{
			minmax[1].x = vertices[i].pos.x;
		}

		if (vertices[i].pos.y > minmax[0].y)
		{
			minmax[0].y = vertices[i].pos.y;
		} else if (vertices[i].pos.y < minmax[1].y)
		{
			minmax[1].y = vertices[i].pos.y;
		}

		if (vertices[i].pos.z > minmax[0].z)
		{
			minmax[0].z = vertices[i].pos.z;
		} else if (vertices[i].pos.z < minmax[1].z)
		{
			minmax[1].z = vertices[i].pos.z;
		}
	}

	min = minmax[1];
	max = minmax[0];
}

int MeshCollider::OctreeSet(const XMFLOAT3& _pos)
{
	int octtreenum[2] = {};
	for (int i = 0; i < 8; i++)
	{
		if (octtreeRange[i].x < _pos.x && _pos.x < octtreeRange[i + 1].x)
		{
			octtreenum[0] = i;
			break;
		}
	}
	for (int i = 0; i < 8; i++)
	{
		if (octtreeRange[i].z < _pos.z && _pos.z < octtreeRange[i + 1].z)
		{
			octtreenum[1] = i;
			break;
		}
	}

	std::string binaryX = std::bitset<3>(octtreenum[0]).to_string();
	std::string binaryZ = std::bitset<3>(octtreenum[1]).to_string();

	std::string nowBinary =
		binaryZ.substr(0, 1) + binaryX.substr(0, 1) +
		binaryZ.substr(1, 1) + binaryX.substr(1, 1) +
		binaryZ.substr(2, 1) + binaryX.substr(2, 1);

	int Octree = strtoul(nowBinary.c_str(), NULL, 2);

	return Octree;
}

void MeshCollider::ConstructTriangles(Model* _model)
{
	if (!isInit)
	{
		object = std::make_unique<PrimitiveObject3D>();
		isInit = true;
	}

	for (int i = 0; i < octtreeSplit; i++)
	{
		triangles[i].clear();
	}
	const std::vector<Mesh*>& meshes = _model->GetMeshes();

	int start = 0;

	MinMax(_model);

	XMFLOAT3 minmaxRange = { max.x - min.x,max.y - min.y,max.z - min.z };
	for (int i = 0; i < 9; i++)
	{
		octtreeRange[i].x = minmaxRange.x / 8 * i;
		octtreeRange[i].y = minmaxRange.y / 8 * i;
		octtreeRange[i].z = minmaxRange.z / 8 * i;
	}

	std::vector<Mesh*>::const_iterator it = meshes.cbegin();
	for (; it != meshes.cend(); ++it) {
		Mesh* mesh = *it;
		const std::vector<Mesh::VERTEX>& vertices = mesh->GetVertices();
		const std::vector<unsigned long>& indices = mesh->GetIndices();

		size_t triangleNum = indices.size() / 3;

		for (int i = 0; i < triangleNum; i++)
		{
			Triangle tri;
			int idx0 = indices[i * 3 + 0];
			int idx1 = indices[i * 3 + 1];
			int idx2 = indices[i * 3 + 2];

			tri.p0 = {
				vertices[idx0].pos.x,
				vertices[idx0].pos.y,
				vertices[idx0].pos.z,
				1 };
			tri.p1 = {
				vertices[idx1].pos.x,
				vertices[idx1].pos.y,
				vertices[idx1].pos.z,
				1 };

			tri.p2 = {
				vertices[idx2].pos.x,
				vertices[idx2].pos.y,
				vertices[idx2].pos.z,
				1 };

			tri.ComputeNormal();

			int Octree1 = OctreeSet(vertices[idx0].pos);
			int Octree2 = OctreeSet(vertices[idx1].pos);
			int Octree3 = OctreeSet(vertices[idx2].pos);

			triangles[Octree1].emplace_back(tri);
			if (Octree1 != Octree2)
			{
				triangles[Octree2].emplace_back(tri);
			}
			if (Octree1 != Octree3 && Octree2 != Octree3)
			{
				triangles[Octree3].emplace_back(tri);
			}

			object->SetVertex(vertices[idx0].pos);
			object->SetVertex(vertices[idx1].pos);
			object->SetVertex(vertices[idx2].pos);
		}
	}
}

void MeshCollider::ConstructTriangles(const std::vector<Mesh::VERTEX>* _vertices, std::vector<unsigned long>* _indices)
{
	if (!isInit)
	{
		object = std::make_unique<PrimitiveObject3D>();
		isInit = true;
	}

	for (int i = 0; i < octtreeSplit; i++)
	{
		triangles[i].clear();
	}

	int start = 0;

	MinMax(_vertices);

	XMFLOAT3 minmaxRange = { max.x - min.x,max.y - min.y,max.z - min.z };
	for (int i = 0; i < 9; i++)
	{
		octtreeRange[i].x = minmaxRange.x / 8 * i;
		octtreeRange[i].y = minmaxRange.y / 8 * i;
		octtreeRange[i].z = minmaxRange.z / 8 * i;
	}

	const std::vector<Mesh::VERTEX> vertices = *_vertices;
	const std::vector<unsigned long> indices = *_indices;

	size_t triangleNum = indices.size() / 3;

	for (int i = 0; i < triangleNum; i++) {

		Triangle tri;
		int idx0 = indices[i * 3 + 0];
		int idx1 = indices[i * 3 + 1];
		int idx2 = indices[i * 3 + 2];

		tri.p0 = {
			vertices[idx0].pos.x,
			vertices[idx0].pos.y,
			vertices[idx0].pos.z,
			1 };
		tri.p1 = {
			vertices[idx1].pos.x,
			vertices[idx1].pos.y,
			vertices[idx1].pos.z,
			1 };

		tri.p2 = {
			vertices[idx2].pos.x,
			vertices[idx2].pos.y,
			vertices[idx2].pos.z,
			1 };

		tri.ComputeNormal();

		int Octree1 = OctreeSet(vertices[idx0].pos);
		int Octree2 = OctreeSet(vertices[idx1].pos);
		int Octree3 = OctreeSet(vertices[idx2].pos);

		triangles[Octree1].emplace_back(tri);
		if (Octree1 != Octree2)
		{
			triangles[Octree2].emplace_back(tri);
		}
		if (Octree1 != Octree3 && Octree2 != Octree3)
		{
			triangles[Octree3].emplace_back(tri);
		}

		object->SetVertex(vertices[idx0].pos);
		object->SetVertex(vertices[idx1].pos);
		object->SetVertex(vertices[idx2].pos);
	}
}

void MeshCollider::Update()
{
	matWorld = GetObject3d()->GetMatWorld();
	invMatWorld = XMMatrixInverse(nullptr, matWorld);
	if (isInit && !isCreateBuffer)
	{
		object->Initialize();
	}
}

void MeshCollider::Draw()
{
	object->SetMatWorld(GetObject3d()->GetMatWorld());
	object->Update();
	object->Draw();
}

bool MeshCollider::CheckCollisionSphere(const Sphere& _sphere, DirectX::XMVECTOR* _inter, DirectX::XMVECTOR* _reject)
{
	// オブジェクトのローカル座標系での球を得る（半径はXスケールを参照)
	Sphere localSphere;
	localSphere.center = XMVector3Transform(_sphere.center, invMatWorld);
	localSphere.radius *= XMVector3Length(invMatWorld.r[0]).m128_f32[0];

	for (auto& itr : triangles)
	{
		std::vector<Triangle>::const_iterator it = itr.cbegin();

		for (; it != itr.cend(); ++it) {
			const Triangle& triangle = *it;

			if (Collision::CheckSphere2Triangle(localSphere, triangle, _inter, _reject)) {
				if (_inter) {
					const XMMATRIX& matWorld = GetObject3d()->GetMatWorld();

					*_inter = XMVector3Transform(*_inter, matWorld);
				}
				if (_reject) {
					const XMMATRIX& matWorld = GetObject3d()->GetMatWorld();

					*_reject = XMVector3TransformNormal(*_reject, matWorld);
				}
				return true;
			}
		}
	}
	return false;
}

bool MeshCollider::CheckCollisionRay(const Ray& _ray, float* _distance, DirectX::XMVECTOR* _inter)
{
	// オブジェクトのローカル座標系でのレイを得る
	Ray localRay;
	localRay.start = XMVector3Transform(_ray.start, invMatWorld);
	localRay.dir = XMVector3TransformNormal(_ray.dir, invMatWorld);

	XMVECTOR vecmax = { max.x,max.y ,max.z,1 };
	XMVECTOR vecmin = { min.x,min.y ,min.z,1 };
	XMVECTOR worldmax = XMVector3Transform(vecmax, matWorld);
	XMVECTOR worldmin = XMVector3Transform(vecmin, matWorld);

	if (_ray.start.m128_f32[0] < worldmin.m128_f32[0] || _ray.start.m128_f32[0] > worldmax.m128_f32[0] ||
		_ray.start.m128_f32[2] <  worldmin.m128_f32[2] || _ray.start.m128_f32[2] >  worldmax.m128_f32[2])
	{
		return false;
	}

	//プレイヤーの八分木位置
	const int Octree = OctreeSet({ localRay.start.m128_f32[0],localRay.start.m128_f32[1],localRay.start.m128_f32[2] });

	std::vector<Triangle>::const_iterator it = triangles[Octree].cbegin();

	for (; it != triangles[Octree].cend(); ++it) {
		const Triangle& triangle = *it;

		XMVECTOR tempInter;

		if (Collision::CheckRay2Triangle(localRay, triangle, nullptr, &tempInter)) {

			const XMMATRIX& matWorld = GetObject3d()->GetMatWorld();

			tempInter = XMVector3Transform(tempInter, matWorld);

			if (_distance) {
				XMVECTOR sub = tempInter - _ray.start;
				*_distance = XMVector3Dot(sub, _ray.dir).m128_f32[0];
			}

			if (_inter) {
				*_inter = tempInter;
			}

			return true;
		}
	}

	return false;
}