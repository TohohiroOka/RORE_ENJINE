#pragma once

#include "BaseCollider.h"
#include <DirectXMath.h>

class Base3D;

/// <summary>
/// レイキャストによる情報を得る為の構造体
/// </summary>
struct RAYCAST_HIT
{
	// 衝突相手のオブジェクト
	Base3D* object = nullptr;
	// 衝突相手のコライダー
	BaseCollider* collider = nullptr;
	// 衝突点
	DirectX::XMVECTOR inter;
	// 衝突点までの距離
	float distance = 0.0f;
};

