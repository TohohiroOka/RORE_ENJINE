#include "Collision.h"

using namespace DirectX;

bool Collision::CheckCircle2Circle(const DirectX::XMFLOAT3& pos1, float radius1, const DirectX::XMFLOAT3& pos2, float radius2)
{
	float disX = pos2.x - pos1.x;
	float disY = pos2.y - pos1.y;
	float disZ = pos2.z - pos1.z;
	float d = disX * disX + disY * disY + disZ * disZ;
	float r = radius1 + radius2;
	return d < r* r;
}

void Collision::ClosestPtPoint2Triangle(const DirectX::XMVECTOR& _point, const Triangle& _triangle, DirectX::XMVECTOR* _closest)
{
	// pointがp0の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p0_p1 = _triangle.p1 - _triangle.p0;
	XMVECTOR p0_p2 = _triangle.p2 - _triangle.p0;
	XMVECTOR p0_pt = _point - _triangle.p0;

	float d1 = XMVector3Dot(p0_p1,p0_pt).m128_f32[0];
	float d2 = XMVector3Dot(p0_p2,p0_pt).m128_f32[0];

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		// p0が最近傍
		*_closest = _triangle.p0;
		return;
	}

	// pointがp1の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p1_pt = _point - _triangle.p1;

	float d3 = XMVector3Dot(p0_p1,p1_pt).m128_f32[0];
	float d4 = XMVector3Dot(p0_p2,p1_pt).m128_f32[0];

	if (d3 >= 0.0f && d4 <= d3)
	{
		// p1が最近傍
		*_closest = _triangle.p1;
		return;
	}

	// pointがp0_p1の辺領域の中にあるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		*_closest = _triangle.p0 + v * p0_p1;
		return;
	}

	// pointがp2の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p2_pt = _point - _triangle.p2;

	float d5 = XMVector3Dot(p0_p1,p2_pt).m128_f32[0];
	float d6 = XMVector3Dot(p0_p2,p2_pt).m128_f32[0];
	if (d6 >= 0.0f && d5 <= d6)
	{
		*_closest = _triangle.p2;
		return;
	}

	// pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		*_closest = _triangle.p0 + w * p0_p2;
		return;
	}

	// pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*_closest = _triangle.p1 + w * (_triangle.p2 - _triangle.p1);
		return;
	}

	// pointは面領域の中にある。closestを重心座標を用いて計算する
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*_closest = _triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphere2Sphere(const Sphere& _sphereA, const Sphere& _sphereB, DirectX::XMVECTOR* _inter, DirectX::XMVECTOR* _reject)
{
	// 中心点の距離の２乗 <= 半径の和の２乗　なら交差
	float dist = XMVector3LengthSq(_sphereA.center - _sphereB.center).m128_f32[0];

	float radius2 = _sphereA.radius + _sphereB.radius;
	radius2 *= radius2;

	if (dist <= radius2) {
		if (_inter) {
			// Aの半径が0の時座標はBの中心　Bの半径が0の時座標はAの中心　となるよう補完
			float t = _sphereB.radius / (_sphereA.radius + _sphereB.radius);
			*_inter = XMVectorLerp(_sphereA.center, _sphereB.center, t);
		}
		// 押し出すベクトルを計算
		if (_reject) {
			float rejectLen = _sphereA.radius + _sphereB.radius - sqrtf(dist);
			*_reject = XMVector3Normalize(_sphereA.center - _sphereB.center);
			*_reject *= rejectLen;
		}
		return true;
	}

	return false;
}

bool Collision::CheckSphere2Plane(const Sphere& _sphere, const Plane& _plane, DirectX::XMVECTOR* _inter)
{
	// 座標系の原点から球の中心座標への距離から
	// 平面の原点距離を減算することで、平面と球の中心との距離が出る
	float dist = XMVector3Dot(_sphere.center, _plane.normal).m128_f32[0] - _plane.distance;
	// 距離の絶対値が半径より大きければ当たっていない
	if (fabsf(dist) > _sphere.radius)	return false;

	// 擬似交点を計算
	if (_inter)
	{
		// 平面上の再接近点を、疑似交点とする
		*_inter = -dist * _plane.normal + _sphere.center;                                                                                              + _sphere.center;
	}

	return true;
}

bool Collision::CheckSphere2Triangle(const Sphere& _sphere, const Triangle& _triangle, DirectX::XMVECTOR* _inter, DirectX::XMVECTOR* _reject)
{
	XMVECTOR p;
	// 球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(_sphere.center, _triangle, &p);
	// 点pと球の中心の差分ベクトル
	XMVECTOR v = p - _sphere.center;
	// 距離の二乗を求める
	//（同じベクトル同士の内積は三平方の定理のルート内部の式と一致する）
	float distanceSquare = XMVector3Dot(v, v).m128_f32[0];
	// 球と三角形の距離が半径以下なら当たっていない
	if (distanceSquare > _sphere.radius * _sphere.radius)	return false;
	// 擬似交点を計算
	if (_inter) {
		// 三角形上の最近接点pを疑似交点とする
		*_inter = p;
	}
	// 押し出すベクトルを計算
	if (_reject) {
		float ds = XMVector3Dot(_sphere.center, _triangle.normal).m128_f32[0];
		float dt = XMVector3Dot(_triangle.p0, _triangle.normal).m128_f32[0];
		float rejectLen = dt - ds + _sphere.radius;
		*_reject = _triangle.normal * rejectLen;
	}
	return true;
}

bool Collision::CheckRay2Plane(const Ray& _lay, const Plane& _plane, float* _distance, DirectX::XMVECTOR* _inter)
{
	const float epsilon = 1.0e-5f;	// 誤差吸収用の微小な値

	float d1 = XMVector3Dot(_plane.normal, _lay.dir).m128_f32[0];
	// 裏面には当たらない
	if (d1 > -epsilon) {
		return false;
	}
	
	float d2 = XMVector3Dot(_plane.normal, _lay.start).m128_f32[0];
	float t = (_plane.distance - d2) / d1;

	if (t < 0) return false;

	// 距離を書き込む
	if (_distance) {
		*_distance = t;
	}

	// 交点を計算
	if (_inter) {
		*_inter = _lay.start + t * _lay.dir;
	}

	return true;
}

bool Collision::CheckRay2Triangle(const Ray& _lay, const Triangle& _triangle, float* _distance, DirectX::XMVECTOR* _inter)
{
	// 三角形が乗っている平面を算出
	Plane plane;
	XMVECTOR interPlane;
	plane.normal = _triangle.normal;
	plane.distance = XMVector3Dot(_triangle.normal, _triangle.p0).m128_f32[0];
	// レイと平面が当たっていなければ、当たっていない	
	if (!CheckRay2Plane(_lay, plane, _distance, &interPlane)) {
		return false;
	}
	// レイと平面が当たっていたので、距離と交点が書き込まれた

	// レイと平面の交点が三角形の内側にあるか判定
	const float epsilon = 1.0e-5f;	// 誤差吸収用の微小な値
	XMVECTOR m;
	// 辺p0_p1について
	XMVECTOR pt_p0 = _triangle.p0 - interPlane;
	XMVECTOR p0_p1 = _triangle.p1 - _triangle.p0;
	m = XMVector3Cross(pt_p0, p0_p1);
	// 辺の外側
	if (XMVector3Dot(m, _triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	// 辺p1_p2について
	XMVECTOR pt_p1 = _triangle.p1 - interPlane;
	XMVECTOR p1_p2 = _triangle.p2 - _triangle.p1;
	m = XMVector3Cross(pt_p1, p1_p2);
	// 辺の外側
	if (XMVector3Dot(m, _triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	// 辺p2_p0について
	XMVECTOR pt_p2 = _triangle.p2 - interPlane;
	XMVECTOR p2_p0 = _triangle.p0 - _triangle.p2;
	m = XMVector3Cross(pt_p2, p2_p0);
	// 辺の外側
	if (XMVector3Dot(m, _triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	if (_inter) {
		*_inter = interPlane;
	}

	// 内側なので、当たっている
	return true;
}

bool Collision::CheckRay2Sphere(const Ray & lay, const Sphere & sphere, float*distance, DirectX::XMVECTOR * inter)
{
	XMVECTOR m = lay.start - sphere.center;
	float b = XMVector3Dot(m, lay.dir).m128_f32[0];
	float c = XMVector3Dot(m, m).m128_f32[0] - sphere.radius * sphere.radius;
	// layの始点がsphereの外側にあり(c > 0)、layがsphereから離れていく方向を
	// 差している場合(b > 0)、当たらない
	if (c > 0.0f && b > 0.0f) {
		return false;
	}

	float discr = b * b - c;
	// 負の判別式はレイが球を外れていることに一致
	if (discr < 0.0f) {
		return false;
	}

	// レイは球と交差している。
	// 交差する最小の値tを計算
	float t = -b - sqrtf(discr);
	// tが負である場合、レイは球の内側から開始しているのでtをゼロにクランプ
	if (t < 0) t = 0.0f;
	
	if (distance) {
		*distance = t;
	}

	if (inter) {
		*inter = lay.start + t * lay.dir;
	}

	return true;
}
