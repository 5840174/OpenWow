#pragma once

#include "M2_Types.h"
#include "M2_Part_Bone.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

// Defines
#define MAX_PARTICLES 100

#include "M2_Particle.h"

struct TexCoordSet 
{
	glm::vec2 tc[4];
};

class SM2_ParticleSystem_Wrapper
{
public:
	SM2_ParticleSystem_Wrapper(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Particle& M2Particle);
	virtual ~SM2_ParticleSystem_Wrapper();

	void update(const CM2_Base_Instance* M2Instance, const UpdateEventArgs& e, float* rem, CM2_ParticleObject* Particles) const;

	SM2_Particle::Flags               GetFlags() const { return m_Flags; }
	const glm::vec3&                  GetPosition() const { return m_Position; }
	int16                             GetBone() const { return m_Bone; }
	const std::shared_ptr<ITexture>&  GetTexture() const { return m_Texture; }
	const IBlendState::BlendMode      GetBlendMode() const;
	const std::vector<TexCoordSet>&   GetTiles() const;

protected:
	void               CreateAndDeleteParticles(const CM2_Base_Instance* M2Instance, const UpdateEventArgs& e, float * rem, CM2_ParticleObject * Particles) const;
	CM2_ParticleObject DefaultGenerator_New(const CM2_Base_Instance* M2Instance, float w, float l, float spd, float var, float lifespan, float spr, float spr2) const;
	CM2_ParticleObject PlaneGenerator_New(const CM2_Base_Instance* M2Instance, float w, float l, float spd, float var, float lifespan, float spr, float spr2) const;
	CM2_ParticleObject SphereGenerator_New(const CM2_Base_Instance* M2Instance, float w, float l, float spd, float var, float lifespan, float spr, float spr2) const;
	void               initTile(glm::vec2 *tc, int num);

private:
	SM2_Particle::Flags       m_Flags;
	glm::vec3                 m_Position;
	int16                     m_Bone;
	std::shared_ptr<ITexture> m_Texture;

	uint8                     m_BlendType;
	uint8                     m_EmitterType;

	uint8                     m_ParticleType;

	M2_Animated<float>        emissionSpeed;
	M2_Animated<float>        speedVariation;
	M2_Animated<float>        verticalRange;
	M2_Animated<float>        horizontalRange;
	M2_Animated<float>        gravity;
	M2_Animated<float>        lifespan;
	M2_Animated<float>        emissionRate;
	M2_Animated<float>        emissionAreaLength;
	M2_Animated<float>        emissionAreaWidth;
	M2_Animated<float>        zSource;

	float                     m_MiddleTime;
	glm::vec4                 m_Colors[3];
	float                     m_Scales[3];

	float                     m_Slowdown;

private:
	M2_Animated<uint8>        enabled;
	int                       order;

	int rows, cols;
	bool billboard;
	std::vector<TexCoordSet> tiles;

private:
	const CM2& m_M2Object;
	const SM2_Particle m_M2Particle;
};
