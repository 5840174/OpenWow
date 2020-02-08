#pragma once

// FORWARD BEGIN
class CM2_ParticleSystem;
// FORWARD END

#include "Particle.h"

mat4 CalcSpreadMatrix(float Spread1, float Spread2, float w, float l);

class ParticleEmitter
{
public:
	ParticleEmitter(std::weak_ptr<CM2_ParticleSystem> sys);

	virtual Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2, uint32 _globalTime) = 0;

protected:



	std::weak_ptr<CM2_ParticleSystem> m_ParticleSystem;
};
