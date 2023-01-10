#pragma once
#include "InterfaceScene.h"
#include "Player.h"
#include "BulletManager.h"

class Boss1 : public InterfaceScene
{
public:

	/// <summary>
	/// ‰Šú‰»
	/// </summary>
	void Initialize() override;

	/// <summary>
	///	XV
	/// </summary>
	void Update() override;

	/// <summary>
	/// XV
	/// </summary>
	void CameraUpdate(Camera* camera) override;

	/// <summary>
	/// •`‰æ
	/// </summary>
	void DrawNotPostB() override;

	/// <summary>
	///	•`‰æ
	/// </summary>
	void Draw() override;
	
	/// <summary>
	/// •`‰æ
	/// </summary>
	void DrawNotPostA() override;

	/// <summary>
	///	‰ğ•ú
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// imgui‚Ì•\¦
	/// </summary>
	void ImguiDraw() override;

private:

	//ƒJƒƒ‰‚Ì‰ñ“]
	XMFLOAT2 cameraAngle;

	std::unique_ptr<Player> player = nullptr;
	std::unique_ptr<HeightMap> map=nullptr;
	std::unique_ptr<BulletManager> bullet = nullptr;

	int houkou=1;
};