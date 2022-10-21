#include "BossBulletC.h"
#include "Easing.h"
#include "GameHelper.h"
#include "Quaternion.h"

using namespace DirectX;

BossBulletC::BossBulletC(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos)
{
	timer = 0;
	const int randMax = 50;
	pos = _pos;
	startPos = _pos;

	targetPos.x = _targetPos.x + Randomfloat(randMax) * float(RandomSign());
	targetPos.y = _targetPos.y + Randomfloat(randMax) * float(RandomSign());
	targetPos.z = _targetPos.z + Randomfloat(randMax) * float(RandomSign());

	for (auto& i : easingKind)
	{
		i = EASING(Randomfloat(int(EASING::SIZE)));
	}

	XMFLOAT2 angle = { float((Randomfloat(314 * 2) - 314)) / 100.0f,float((Randomfloat(314 * 2) - 314)) / 100.0f };
	XMFLOAT2 radiun = { XMConvertToRadians(angle.x),XMConvertToRadians(angle.y) };

	moveVec = { cos(radiun.x) * cos(radiun.y), cos(radiun.x) * sin(radiun.y), sin(radiun.x) };
	const float speed = 8.0f;
	move = { moveVec.x * speed,moveVec.y * speed,moveVec.z * speed };

	isStorage = true;
	storagePos = { 0,0,0 };
}

std::unique_ptr<BossBulletC> BossBulletC::Create(const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, const XMFLOAT3& _color)
{
	// 3Dオブジェクトのインスタンスを生成
	BossBulletC* instance = new BossBulletC(_pos, _targetPos);
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	instance->Initialize();
	instance->object->SetColor({ _color.x,_color.y,_color.z,1.0f });
	return std::unique_ptr<BossBulletC>(instance);
}

void BossBulletC::Initialize()
{
	object = Object3d::Create(model.get());

	BaseBullet::Initialize();

	// 速さv、半径rで円を描く時、その向心力はv^2/r。これを計算しておく。
	maxCentripetalAccel = speed * speed / curvatureRadius;
	// 終端速度がspeedになるaccelを求める
	// v = a / kだからa=v*k
	propulsion = speed * damping;
}

void BossBulletC::Update()
{
	const int maxTime = 80;

	if (timer < maxTime)
	{
		timer++;
		float rate = float(timer) / float(maxTime);

		if (rate >= 0.8f && isStorage) {
			storagePos = pos;
			isStorage = false;
		}

		pos.x = EasingDivide(easingKind[0], startPos.x, targetPos.x, rate);
		pos.y = EasingDivide(easingKind[1], startPos.y, targetPos.y, rate);
		pos.z = EasingDivide(easingKind[2], startPos.z, targetPos.z, rate);

		if (timer == maxTime) {
			Vector3 moveVecNotN = { pos.x - storagePos.x, pos.y - storagePos.y,pos.z - storagePos.z };
			moveVecNotN.normalize();
			const float speed = 10.0f;
			move.x = moveVecNotN.x * speed;
			move.y = moveVecNotN.y * speed;
			move.z = moveVecNotN.z * speed;
		}
	}

	{
		//Vector3 moveVec3 = { move.x, move.y, move.z };
		//Vector3 toTarget = { targetPos.x - pos.x,targetPos.y - pos.y,targetPos.z - pos.z };
		//Vector3 vn = moveVec3.normalize();
		//float dot = vn.dot(toTarget);
		//Vector3 centripetalAccel = toTarget - (vn * dot);
		//float centripetalAccelLength = centripetalAccel.length();
		//if (centripetalAccelLength > 1.0f)
		//{
		//	centripetalAccel /= centripetalAccelLength;
		//}
		//Vector3 force = centripetalAccel * maxCentripetalAccel;
		//force += vn * propulsion;
		//force -= moveVec3 * damping;
		//moveVec3 += force;
		//move = { moveVec3.x,moveVec3.y ,moveVec3.z };
	}

	BaseBullet::Update();
}

float BossBulletC::EasingDivide(EASING kind, float _startPos, float _targetPos, float rate)
{
	float returnPos = 0;
	if (kind == EASING::LERP){
		returnPos = Easing::OutQuart(_startPos, _targetPos, rate);
	}
	else if (kind == EASING::IN_QUART) {
		returnPos = Easing::InQuart(_startPos, _targetPos, rate);
	}
	else if (kind == EASING::OUT_QUART) {
		returnPos = Easing::OutQuart(_startPos, _targetPos, rate);
	}
	else if (kind == EASING::IN_EXPO) {
		returnPos = Easing::InExpo(_startPos, _targetPos, rate);
	}
	else if (kind == EASING::OUT_EXPO) {
		returnPos = Easing::OutExpo(_startPos, _targetPos, rate);
	}
	else if (kind == EASING::IN_CIRC) {
		returnPos = Easing::InCirc(_startPos, _targetPos, rate);
	}
	else if (kind == EASING::OUT_CIRC) {
		returnPos = Easing::OutCirc(_startPos, _targetPos, rate);
	}

	return returnPos;
}
