#pragma once

// FORWARD BEGIN
class CM2_ParticleSystem;
// FORWARD END

#include "ParticleEmitters.h"

class SphereParticleEmitter : public ParticleEmitter
{
public:
	SphereParticleEmitter(std::weak_ptr<CM2_ParticleSystem> sys)
		: ParticleEmitter(sys)
	{}

	Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2, uint32 _globalTime) override;
};
