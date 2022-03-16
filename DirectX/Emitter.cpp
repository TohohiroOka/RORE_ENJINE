#include "Emitter.h"
#include "Camera.h"

Emitter::~Emitter()
{
	delete particleManager;
}

void Emitter::Create(int texNumber)
{
	particleManager = new ParticleManager();
	particleManager->Create(texNumber);
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

void Emitter::AllDelete()
{
	particleManager->AllDelete();
}