#include "Emitter.h"
#include "Camera.h"

Emitter::~Emitter()
{
}

std::unique_ptr<Emitter> Emitter::Create(int texNumber)
{
	// 3Dオブジェクトのインスタンスを生成
	Emitter* instance = new Emitter();

	instance->particleManager = ParticleManager::Create(texNumber);

	return std::unique_ptr<Emitter>(instance);
}

void Emitter::InEmitter(int maxCount, int maxFrame, XMFLOAT3 position, XMFLOAT3 velocity,
	XMFLOAT3 accel, float startScale, float endScale, XMFLOAT4 startColor, XMFLOAT4 endColor)
{
	if (maxCount > count)
	{
		particleManager->Add(maxFrame, position, velocity,
			accel, startScale, endScale, startColor, endColor);
	}
}

void Emitter::Update(Camera* camera)
{
	count = particleManager->Update(camera);
}

void Emitter::Draw()
{
	particleManager->Draw();
}

void Emitter::ParticlAllDelete()
{
	particleManager->ParticlAllDelete();
}