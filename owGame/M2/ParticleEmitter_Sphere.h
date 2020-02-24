#pragma once

// FORWARD BEGIN
class M2;
class CM2_ParticleSystem;
// FORWARD END

#include "ParticleEmitters.h"

class SphereParticleEmitter : public ParticleEmitter
{
public:
	SphereParticleEmitter(CM2_ParticleSystem& ParticleSystem)
		: ParticleEmitter(ParticleSystem)
	{}

	Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2, uint32 _globalTime) override;
};
