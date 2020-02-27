#include "stdafx.h"

// Include
#include "M2.h"
#include "ParticleSystem.h"

// General
#include "ParticleEmitter_Plane.h"


Particle PlaneParticleEmitter::newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2, uint32 _globalTime)
{
	std::shared_ptr<const CM2_Part_Bone> ParticleSystem_ParentBone = m_ParticleSystem.m_ParentBone.lock();
	_ASSERT(ParticleSystem_ParentBone != nullptr);

    Random random;

	// Model Flags - *shrug* gotta write this down somewhere.
	// 0x1 =
	// 0x2 =
	// 0x4 =
	// 0x8 = 
	// 0x10 = 
	// 19 = 0x13 = blue ball in thunderfury = should be billboarded?

	// Particle Flags
	// 0x0	/ 0		= Basilisk has no flags?
	// 0x1	/ 1		= Pretty much everything I know of except Basilisks have this flag..  Billboard?
	// 0x2	/ 2		=
	// 0x4	/ 4		=
	// 0x8  / 8		= 
	// 0x10	/ 16	= Position Relative to bone pivot?
	// 0x20	/ 32	=
	// 0x40	/ 64	=
	// 0x80 / 128	=
	// 0x100 / 256	=
	// 0x200 / 512	=
	// 0x400 / 1024 =
	// 0x800 / 2048 =
	// 0x1000/ 4096 =
	// 0x0000/ 1593 = [1,8,16,32,512,1024]"Warp Storm" - aura type particle effect
	// 0x419 / 1049 = [1,8,16,1024] Forest Wind shoulders
	// 0x411 / 1041 = [1,16,1024] Halo
	// 0x000 / 541	= [1,4,8,16,512] Staff glow
	// 0x000 / 537 = "Warp Storm"
	// 0x31 / 49 = [1,16,32] particle moving up?
	// 0x00 / 41 = [1,8,32] Blood elf broom, dust spread out on the ground (X, Z axis)
	// 0x1D / 29 = [1,4,8,16] particle being static
	// 0x19 / 25 = [1,8,16] flame on weapon - move up/along the weapon
	// 17 = 0x11 = [1,16] glow on weapon - static, random direction.  - Aurastone Hammer
	// 1 = 0x1 = perdition blade
	// 4121 = water ele
	// 4097 = water elemental
	// 1041 = Transcendance Halo
	// 1039 = water ele

	Particle p;

	//Spread Calculation
	mat4 mrot;

	mat4 SpreadMat = CalcSpreadMatrix(spr, spr, 1.0f, 1.0f);
	mrot = ParticleSystem_ParentBone->getRotateMatrix() * SpreadMat;

	if (m_ParticleSystem.flags == 1041)
	{ // Trans Halo
		p.pos = ParticleSystem_ParentBone->getTransformMatrix() * vec4((m_ParticleSystem.m_Position + vec3(random.Range(-l, l), 0, random.Range(-w, w))), 0);

		const float t = random.Range(0.0f, glm::two_pi<float>());

		p.pos = vec3(0.0f, m_ParticleSystem.m_Position.y + 0.15f, m_ParticleSystem.m_Position.z) + vec3(cos(t) / 8, 0.0f, sin(t) / 8); // Need to manually correct for the halo - why?

																								 // var isn't being used, which is set to 1.0f,  whats the importance of this?
																								 // why does this set of values differ from other particles

		vec3 dir(0.0f, 1.0f, 0.0f);
		p.dir = dir;

		p.speed = glm::normalize(dir) * spd * random.Range(0.0f, var);
	}
	else if (m_ParticleSystem.flags == 25 && ParticleSystem_ParentBone->getParentBoneID_DirectArray() < 1)
	{ // Weapon Flame
		p.pos = ParticleSystem_ParentBone->getPivot() * (m_ParticleSystem.m_Position + vec3(random.Range(-l, l), random.Range(-l, l), random.Range(-w, w)));
		vec3 dir = mrot * vec4(0.0f, 1.0f, 0.0f, 0.0f);
		p.dir = glm::normalize(dir);
		//vec3 dir = sys->model->bones[sys->parent->parent].mrot * sys->parent->mrot * vec3(0.0f, 1.0f, 0.0f);
		//p.speed = dir.Normalize() * spd;

	}
	else if (m_ParticleSystem.flags == 25 && ParticleSystem_ParentBone->getParentBoneID_DirectArray() > 0)
	{ // Weapon with built-in Flame (Avenger lightsaber!)
		p.pos = ParticleSystem_ParentBone->getTransformMatrix() * vec4((m_ParticleSystem.m_Position + vec3(random.Range(-l, l), random.Range(-l, l), random.Range(-w, w))), 0);
		vec3 dir = vec3
		(
			ParticleSystem_ParentBone->getTransformMatrix()[1][0],
			ParticleSystem_ParentBone->getTransformMatrix()[1][1],
			ParticleSystem_ParentBone->getTransformMatrix()[1][2]
		) * vec3(0.0f, 1.0f, 0.0f);
		p.speed = glm::normalize(dir) * spd * random.Range(0.0f, var * 2);

	}
	else if (m_ParticleSystem.flags == 17 && ParticleSystem_ParentBone->getParentBoneID_DirectArray() < 1)
	{ // Weapon Glow
		p.pos = ParticleSystem_ParentBone->getPivot() * (m_ParticleSystem.m_Position + vec3(random.Range(-l, l), random.Range(-l, l), random.Range(-w, w)));
		vec3 dir = mrot * vec4(0, 1, 0, 0);
		p.dir = glm::normalize(dir);
	}
	else
	{
		p.pos = m_ParticleSystem.m_Position + vec3(random.Range(-l, l), 0, random.Range(-w, w));
		p.pos = ParticleSystem_ParentBone->getTransformMatrix() * vec4(p.pos, 0);

		//vec3 dir = mrot * vec3(0,1,0);
		vec3 dir = ParticleSystem_ParentBone->getRotateMatrix() * vec4(0, 1, 0, 0);

		p.dir = dir;//.Normalize();
		p.down = vec3(0, -1.0f, 0); // dir * -1.0f;
		p.speed = glm::normalize(dir) * spd * (1.0f + random.Range(-var, var));
	}

	if (!m_ParticleSystem.billboard)
	{
		p.corners[0] = mrot * vec4(-1, 0, +1, 0);
		p.corners[1] = mrot * vec4(+1, 0, +1, 0);
		p.corners[2] = mrot * vec4(+1, 0, -1, 0);
		p.corners[3] = mrot * vec4(-1, 0, -1, 0);
	}

	p.life = 0;
	p.maxlife = m_ParticleSystem.lifespan.GetValue(anim, time, m_ParticleSystem.m_M2Object.getSkeleton().getGlobalLoops(), _globalTime);

	p.origin = p.pos;

	p.m_TileExists = random.Range(0, m_ParticleSystem.rows * m_ParticleSystem.cols - 1);
	return p;
}