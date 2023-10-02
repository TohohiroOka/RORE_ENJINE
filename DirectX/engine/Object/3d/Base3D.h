#pragma once
#include "Object/ObjectBase.h"
#include "Model.h"
#include "Object/3d/collider/CollisionInfo.h"

class BaseCollider;
class Camera;
class LightGroup;

class Base3D : public ObjectBase
{
public:

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="_camera">カメラ</param>
	static void SetCamera(Camera* _camera) { Base3D::camera = _camera; }

	/// <summary>
	/// ライトグループのセット
	/// </summary>
	/// <param name="_light">ライトグループ</param>
	static void SetLightGroup(LightGroup* _light) { Base3D::light = _light; }

public:

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 行列の更新
	/// </summary>
	virtual void UpdateWorldMatrix();

	/// <summary>
	/// 当たり判定の描画
	/// </summary>
	virtual void ColliderDraw();

	/// <summary>
	/// コライダーの削除
	/// </summary>
	virtual void DeleteCollider();

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="_info">衝突情報</param>
	virtual void OnCollision(const CollisionInfo& _info) {}

protected:
	//カメラ
	static Camera* camera;
	//ライト
	static LightGroup* light;

	// コライダー
	BaseCollider* collider = nullptr;
	// ローカル座標変換行列
	XMMATRIX matTrans = {};
	// サイズ変換行列
	XMMATRIX matScale = {};
	// 角度変換行列
	XMMATRIX matRot = {};
	// ローカルワールド変換行列
	XMMATRIX matWorld = {};
	// ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };
	// ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	// 親オブジェクト
	Base3D* parent = nullptr;

	//ブルームの有無
	bool isBloom;
	//トゥーンの有無
	bool isToon;
	//アウトラインの有無
	bool isOutline;
	//ライティング
	bool isLight;
	//アウトライン色
	XMFLOAT3 outlineColor;

public:

	const XMFLOAT3& GetPosition() { return position; }
	const XMFLOAT3& GetRotation() { return rotation; }
	const XMFLOAT3& GetScale() { return scale; }
	BaseCollider* GetCollider() { return collider; }
	const XMMATRIX& GetMatRot() { return matRot; }
	const XMMATRIX& GetMatWorld() { return matWorld; }
	const XMFLOAT3 GetWorldPosition();
	bool GetBloom() { return isBloom; }
	void SetParent(Base3D* _parent) { parent = _parent; }
	void SetPosition(const XMFLOAT3& _position) { position = _position; }
	void SetRotation(const XMFLOAT3& _rotation) { rotation = _rotation; }
	void SetScale(const XMFLOAT3& _scale) { scale = _scale; }
	void SetCollider(BaseCollider* _collider);
	void SetMatWorld(const XMMATRIX& _matWorld) { matWorld = _matWorld; }
	void SetBloom(bool _isBloom) { isBloom = _isBloom; }
	void SetToon(bool _isToon) { isToon = _isToon; }
	void SetLight(bool _isLight) { isLight = _isLight; }
	void SetOutline(bool _isOutline) { isOutline = _isOutline; }
	void SetOutlineColor(const XMFLOAT3& _outlineColor) { outlineColor = _outlineColor; }

};