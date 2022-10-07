#include "Emitter.h"
#include "Camera.h"

std::unique_ptr<Emitter> Emitter::Create(const std::string& _name)
{
	// 3Dオブジェクトのインスタンスを生成
	Emitter* instance = new Emitter();

	instance->particleManager = ParticleManager::Create(_name);

	return std::unique_ptr<Emitter>(instance);
}

void Emitter::InEmitter(const int& _maxCount, const int& _maxFrame, const XMFLOAT3& _position, const XMFLOAT3& _velocity,
	const XMFLOAT3& _accel, const float& _startScale, const float& _endScale, const XMFLOAT4& _startColor, const XMFLOAT4& _endColor)
{
	if (_maxCount > count)
	{
		particleManager->Add(_maxFrame, _position, _velocity,
			_accel, _startScale, _endScale, _startColor, _endColor);
	}
}

void Emitter::Update()
{
	count = particleManager->Update();
}

void Emitter::Draw()
{
	particleManager->Draw();
}

void Emitter::ParticlAllDelete()
{
	particleManager->ParticlAllDelete();
}