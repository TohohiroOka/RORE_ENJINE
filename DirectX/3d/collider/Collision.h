#pragma once

#include "CollisionPrimitive.h"

/// <summary>
/// 当たり判定ヘルパークラス
/// </summary>
class Collision
{
public:

	/// <summary>
	/// 円と円の当たり判定
	/// </summary>
	/// <param name="pos1">円1の座標</param>
	/// <param name="radius1">円1の半径</param>
	/// <param name="pos2">円2の座標</param>
	/// <param name="radius2">円2の半径</param>
	/// <returns>交差しているか否か</returns>
	static bool CheckCircle2Circle(const DirectX::XMFLOAT3& pos1, float radius1, const DirectX::XMFLOAT3& pos2, float radius2);

	/// <summary>
	/// 点と三角形の最近接点を求める
	/// </summary>
	/// <param name="_point">点</param>
	/// <param name="_triangle">三角形</param>
	/// <param name="_closest">最近接点（出力用）</param>
	static void ClosestPtPoint2Triangle(const DirectX::XMVECTOR& _point, const Triangle& _triangle, DirectX::XMVECTOR* _closest);

	/// <summary>
	/// 球と球の当たり判定
	/// </summary>
	/// <param name="_sphereA">球A</param>
	/// <param name="_sphereB">球B</param>
	/// <param name="_inter">交点（出力用）</param>
	/// <param name="_reject">排斥ベクトル（出力用）</param>
	/// <returns></returns>
	static bool CheckSphere2Sphere(const Sphere& _sphereA, const Sphere& _sphereB, DirectX::XMVECTOR* _inter = nullptr, DirectX::XMVECTOR* _reject = nullptr);

	/// <summary>
	/// 球と平面の当たり判定
	/// </summary>
	/// <param name="_sphere">球</param>
	/// <param name="_plane">平面</param>
	/// <param name="_inter">交点（平面上の最近接点）</param>
	/// <returns>交差しているか否か</returns>
	static bool CheckSphere2Plane(const Sphere& _sphere, const Plane& _plane, DirectX::XMVECTOR* _inter = nullptr);

	/// <summary>
	/// 球と法線付き三角形の当たり判定
	/// </summary>
	/// <param name="_sphere">球</param>
	/// <param name="_triangle">三角形</param>
	/// <param name="_inter">交点（三角形上の最近接点）</param>
	/// <param name="_reject">排斥ベクトル（出力用）</param>
	/// <returns>交差しているか否か</returns>
	static bool CheckSphere2Triangle(const Sphere& _sphere, const Triangle& _triangle, DirectX::XMVECTOR* _inter = nullptr, DirectX::XMVECTOR* _reject = nullptr);

	/// <summary>
	/// レイと平面の当たり判定
	/// </summary>
	/// <param name="_lay">レイ</param>
	/// <param name="_plane">平面</param>
	/// <param name="_inter">距離（出力用）</param>
	/// <param name="_inter">交点（出力用）</param>
	/// <returns>交差しているか否か</returns>
	static bool CheckRay2Plane(const Ray& _lay, const Plane& _plane, float* _distance = nullptr, DirectX::XMVECTOR* _inter = nullptr);

	/// <summary>
	/// レイと法線付き三角形の当たり判定
	/// </summary>
	/// <param name="_lay">レイ</param>
	/// <param name="_triangle">三角形</param>
	/// <param name="_inter">距離（出力用）</param>
	/// <param name="_inter">交点（出力用）</param>
	/// <returns>交差しているか否か</returns>
	static bool CheckRay2Triangle(const Ray& _lay, const Triangle& _triangle, float* _distance = nullptr, DirectX::XMVECTOR* _inter = nullptr);

	/// <summary>
	/// レイと球の当たり判定
	/// </summary>
	/// <param name="_lay">レイ</param>
	/// <param name="_sphere">球</param>
	/// <param name="_sphere">距離（出力用）</param>
	/// <param name="_inter">交点（出力用）</param>
	/// <returns>交差しているか否か</returns>
	static bool CheckRay2Sphere(const Ray& _lay, const Sphere& _sphere, float* _distance = nullptr, DirectX::XMVECTOR* _inter = nullptr);
};