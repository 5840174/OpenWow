#pragma once

#include "Particle.h"

#include "M2_Types.h"
#include "M2_Part_Bone.h"

typedef list<Particle> ParticleList;

// Defines
#define MAX_PARTICLES 10000


struct TexCoordSet
{
	vec2 tc[4];
};

#include "ParticleEmitters.h"

class CM2_ParticleSystem
{
	friend class ParticleEmitter;
	friend class PlaneParticleEmitter;
	friend class SphereParticleEmitter;
public:
	CM2_ParticleSystem(M2* _parentM2, IFile* f, const SM2_Particle& mta, cGlobalLoopSeq globals);
	~CM2_ParticleSystem();

	void update(double _time, double _dTime);
	void setup(uint16 anim, uint32 time, uint32 _globalTime);

	void Render3D(cmat4 _worldMatrix);

private:

	ParticleEmitter* m_Emitter;
	SharedTexturePtr texture;

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
	vector<TexCoordSet> m_Tiles;

	bool billboard;

	float rem;
	//bool transform;

	// unknown parameters omitted for now ...
	int32 flags;
	int16 pType;

	RenderState							m_State;

private:
	const M2*             m_ParentM2;
	const CM2_Part_Bone*  m_ParentBone;
};

template<class T>
inline T lifeRamp(float life, float mid, const T &a, const T &b, const T &c)
{
	if (life <= mid)
		return interpolate<T>(life / mid, a, b);
	else
		return interpolate<T>((life - mid) / (1.0f - mid), b, c);
}