#include "stdafx.h"

// Include
#include "M2.h"
#include "ParticleSystem.h"

// General
#include "ParticleEmitter_Sphere.h"

Particle SphereParticleEmitter::newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2, uint32 _globalTime)
{
	std::shared_ptr<const CM2_Part_Bone> ParticleSystem_ParentBone = m_ParticleSystem.m_ParentBone.lock();
	_ASSERT(ParticleSystem_ParentBone != nullptr);

    Random random;

	Particle p;
	vec3 dir;
	float radius;

	radius = random.Range(0.0f, 1.0f);

	// Old method
	//float t = Random::GenerateRange(0,2*Math::Pi);

	// New
	// Spread should never be zero for sphere particles ?
	float t = 0;
	if (spr == 0)
		t = random.Range(-glm::pi<float>(), glm::pi<float>());
	else
		t = random.Range(-spr, spr);

	//Spread Calculation
	mat4 mrot;

	mat4 SpreadMat = CalcSpreadMatrix(spr * 2, spr2 * 2, w, l);
	mrot = ParticleSystem_ParentBone->getRotateMatrix() * SpreadMat;

	// New
	// Length should never technically be zero ?
	//if (l==0)
	//	l = w;

	// New method
	// vec3 bdir(w*cosf(t), 0.0f, l*sinf(t));
	// --

	// TODO: fix shpere emitters to work properly
	/* // Old Method
	//vec3 bdir(l*cosf(t), 0, w*sinf(t));
	//vec3 bdir(0, w*cosf(t), l*sinf(t));


	float theta_range = sys->spread.getValue(anim, time);
	float theta = -0.5f* theta_range + Random::GenerateRange(0, theta_range);
	vec3 bdir(0, l*cosf(theta), w*sinf(theta));

	float phi_range = sys->lat.getValue(anim, time);
	float phi = Random::GenerateRange(0, phi_range);
	rotate(0,0, &bdir.z, &bdir.x, phi);
	*/

	if (m_ParticleSystem.flags == 57 || m_ParticleSystem.flags == 313)
	{ // Faith Halo
		vec3 bdir(w*cosf(t)*1.6, 0.0f, l*sinf(t)*1.6);

		p.pos = m_ParticleSystem.m_Position + bdir;
		p.pos = ParticleSystem_ParentBone->getTransformMatrix() * vec4(p.pos, 0);

		if (glm::length2(bdir) == 0)
			p.speed = vec3(0, 0, 0);
		else
		{
			dir = ParticleSystem_ParentBone->getRotateMatrix() * vec4((glm::normalize(bdir)), 0);//mrot * vec3(0, 1.0f,0);
			p.speed = glm::normalize(dir) * spd * (1.0f + random.Range(-var, var));   // ?
		}

	}
	else
	{
		vec3 bdir;
		float temp;

		bdir = mrot * vec4(vec3(0, 1, 0) * radius, 0);
		temp = bdir.z;
		bdir.z = bdir.y;
		bdir.y = temp;

		p.pos = ParticleSystem_ParentBone->getTransformMatrix() * vec4(m_ParticleSystem.m_Position + bdir, 0);


		//p.pos = sys->pos + bdir;
		//p.pos = sys->parent->mat * p.pos;


		if ((glm::length2(bdir) == 0) && ((m_ParticleSystem.flags & 0x100) != 0x100))
		{
			p.speed = vec3(0, 0, 0);
			dir = ParticleSystem_ParentBone->getRotateMatrix() * vec4(0, 1, 0, 0);
		}
		else
		{
			if (m_ParticleSystem.flags & 0x100)
				dir = ParticleSystem_ParentBone->getRotateMatrix() * vec4(0, 1, 0, 0);
			else
				dir = glm::normalize(bdir);

			p.speed = glm::normalize(dir) * spd * (1.0f + random.Range(-var, var));   // ?
		}
	}

	p.dir = glm::normalize(dir);//mrot * vec3(0, 1.0f,0);
	p.down = vec3(0, -1.0f, 0);

	p.life = 0;
	p.maxlife = m_ParticleSystem.lifespan.GetValue(anim, time, m_ParticleSystem.m_M2Object.getGlobalLoops(), _globalTime);

	p.origin = p.pos;

	p.m_TileExists = random.Range(0, m_ParticleSystem.rows * m_ParticleSystem.cols - 1);
	return p;
}
