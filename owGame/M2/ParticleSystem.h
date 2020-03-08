#pragma once

#include "Particle.h"

#include "M2_Types.h"
#include "M2_Part_Bone.h"

typedef std::list<Particle> ParticleList;

// Defines
#define MAX_PARTICLES 10000


struct TexCoordSet
{
	vec2 tc[4];
};

#include "ParticleEmitters.h"

class CM2_ParticleSystem 
	: public std::enable_shared_from_this<CM2_ParticleSystem>
{
	friend class ParticleEmitter;
	friend class PlaneParticleEmitter;
	friend class SphereParticleEmitter;
public:
	CM2_ParticleSystem(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Particle& M2Particle);

	void update(double _time, double _dTime);
	void setup(uint16 anim, uint32 time, uint32 _globalTime);

	void Render3D(cmat4 _worldMatrix);

private:

	std::shared_ptr<ParticleEmitter> m_Emitter;
	std::shared_ptr<ITexture> texture;

private:
	void initTile(vec2 *tc, int num);

private:
	M2_Animated<float> emissionSpeed, speedVariation, verticalRange, horizontalRange, gravity, lifespan, emissionRate, emissionAreaLength, emissionAreaWidth, zSource;
	M2_Animated<uint8> enabled;
	vec4 colors[3];
	float sizes[3];
	float mid, slowdown, rotation;
	vec3 m_Position;
	
	
	ParticleList particles;
	int blend, order, type;


	uint32 m_CurrentAnimation, m_CurrentTime, m_GlobalTime;
	int rows, cols;
	std::vector<TexCoordSet> m_Tiles;

	bool billboard;

	float rem;
	//bool transform;

	// unknown parameters omitted for now ...
	int32 flags;
	int16 pType;

private:
	const CM2& m_M2Object;
	int16 m_ParentBone;
};

template<class T>
inline T lifeRamp(float life, float mid, const T &a, const T &b, const T &c)
{
	if (life <= mid)
		return interpolate<T>(life / mid, a, b);
	else
		return interpolate<T>((life - mid) / (1.0f - mid), b, c);
}