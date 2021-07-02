#pragma once

#include "M2_Types.h"
#include "M2_Part_Bone.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

// Defines
#define MAX_PARTICLES 100

#include "M2_Part_Particle.h"

/*struct CompressedParticleGravity
{
	int8_t x, y;
	int16_t z;
};

class CompressedParticleGravityToVec3
{
public:
	static const glm::vec3 conv(const float t)
	{
		const CompressedParticleGravity compressedGravity = reinterpret_cast<const CompressedParticleGravity&>(t);

		glm::vec3 dir = glm::vec3(compressedGravity.x, compressedGravity.y, 0) * (1.0f / 128.0f);
		float z = sqrtf(1.0f - glm::dot(dir, dir));
		float mag = compressedGravity.z * 0.04238648f;
		if (mag < 0)
		{
			z = -z;
			mag = -mag;
		}
		dir.z = z;
		dir *= mag;
		return dir;
	}
};*/


struct SM2ParticleTileCoords 
{
	glm::vec2 Start;
	glm::vec2 End;
};


class CM2_Part_ParticleSystem
{
public:
	CM2_Part_ParticleSystem(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Particle& M2Particle);
	virtual ~CM2_Part_ParticleSystem();

	void update(const CM2_Base_Instance* M2Instance, const UpdateEventArgs& e, float* rem, SGPUM2Particle* Particles) const;

	SM2_Particle::Flags               GetFlags() const { return m_M2Particle.flags; }
	const glm::vec3&                  GetPosition() const { return m_Position; }
	int16                             GetBone() const { return m_M2Particle.bone; }
	const std::shared_ptr<ITexture>&  GetTexture() const { return m_Texture; }
	bool                              IsBillboard() const { return false == (m_M2Particle.flags.DONOTBILLBOARD); }
	const IBlendState::BlendMode      GetBlendMode() const;
	const std::vector<SM2ParticleTileCoords>&   GetTiles() const;

protected:
	void               CreateAndDeleteParticles(const CM2_Base_Instance* M2Instance, const UpdateEventArgs& e, float * rem, SGPUM2Particle * Particles) const;
	SGPUM2Particle DefaultGenerator_New(const CM2_Base_Instance* M2Instance, float w, float l, float spd, float var, float lifespan, float spr, float spr2) const;
	SGPUM2Particle PlaneGenerator_New(const CM2_Base_Instance* M2Instance, float w, float l, float spd, float var, float lifespan, float spr, float spr2) const;
	SGPUM2Particle SphereGenerator_New(const CM2_Base_Instance* M2Instance, float w, float l, float spd, float var, float lifespan, float spr, float spr2) const;
	void               InitTiles();

private:
	glm::vec3                 m_Position;
	std::shared_ptr<ITexture> m_Texture;

	M2_Animated<float>        emissionSpeed;
	M2_Animated<float>        speedVariation;
	M2_Animated<float>        verticalRange;
	M2_Animated<float>        horizontalRange;
	M2_Animated<float>        m_Gravity;
	M2_Animated<float>        lifespan;
	M2_Animated<float>        emissionRate;
	M2_Animated<float>        emissionAreaLength;
	M2_Animated<float>        emissionAreaWidth;
	M2_Animated<float>        m_zSource;

	float                     m_MiddleTime;
	glm::vec4                 m_Colors[3];
	float                     m_Scales[3];

private:
	M2_Animated<uint8>        enabled;

	std::vector<SM2ParticleTileCoords>  tiles;

private:
	const CM2&                m_M2Object;
	const SM2_Particle        m_M2Particle;
};
