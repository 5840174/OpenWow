#include "stdafx.h"

// Inlcudes
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Part_ParticleSystem.h"

CM2_Part_ParticleSystem::CM2_Part_ParticleSystem(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Particle& M2Particle)
	: m_M2Object(M2Object)
	, m_M2Particle(M2Particle)
{
	m_Position = Fix_XZmY(M2Particle.Position);
	m_Texture = m_M2Object.getMaterials().GetTextureDirectInternal(M2Particle.texture)->GetTexture();
	
	emissionSpeed.Initialize(M2Particle.emissionSpeed, File, M2Object.getSkeleton().GetAnimFiles());
	speedVariation.Initialize(M2Particle.speedVariation, File, M2Object.getSkeleton().GetAnimFiles());
	verticalRange.Initialize(M2Particle.verticalRange, File, M2Object.getSkeleton().GetAnimFiles());
	horizontalRange.Initialize(M2Particle.horizontalRange, File, M2Object.getSkeleton().GetAnimFiles());
	
	if (GetFlags().IsGravityCompressed)
		throw CException("ParticleCompressedGravity");

	m_Gravity.Initialize(M2Particle.gravity, File, M2Object.getSkeleton().GetAnimFiles());

	lifespan.Initialize(M2Particle.lifespan, File, M2Object.getSkeleton().GetAnimFiles());
	emissionRate.Initialize(M2Particle.emissionRate, File, M2Object.getSkeleton().GetAnimFiles());
	emissionAreaLength.Initialize(M2Particle.emissionAreaLength, File, M2Object.getSkeleton().GetAnimFiles());
	emissionAreaWidth.Initialize(M2Particle.emissionAreaWidth, File, M2Object.getSkeleton().GetAnimFiles());
	m_zSource.Initialize(M2Particle.zSource, File, M2Object.getSkeleton().GetAnimFiles());
	
	enabled.Initialize(M2Particle.enabledIn, File, M2Object.getSkeleton().GetAnimFiles());

#if WOW_CLIENT_VERSION < WOW_WOTLK_3_3_5
	m_MiddleTime = M2Particle.midPoint;
	for (size_t i = 0; i < 3; i++)
	{
		m_Colors[i] = glm::vec4(M2Particle.colorValues[i].r, M2Particle.colorValues[i].g, M2Particle.colorValues[i].b, M2Particle.colorValues[i].a) / 255.0f;
		m_Scales[i]  = M2Particle.scaleValues[i];
	}
#else
	m_MiddleTime = 0.5f;

	glm::vec3 colors2[3];
	memcpy(colors2, File->getData() + M2Particle.colorTrack.values.offset, sizeof(glm::vec3) * 3);

	for (uint32 i = 0; i < 3; i++)
	{
		float opacity = *(short*)(File->getData() + M2Particle.alphaTrack.values.offset + i * sizeof(short));

		m_Colors[i] = glm::vec4(colors2[i].x / 255.0f, colors2[i].y / 255.0f, colors2[i].z / 255.0f, opacity / 32767.0f);
		m_Scales[i] = (*(float*)(File->getData() + M2Particle.scaleTrack.values.offset + i * sizeof(float)));
	}
#endif

	InitTiles();
}

CM2_Part_ParticleSystem::~CM2_Part_ParticleSystem()
{
}

void CM2_Part_ParticleSystem::update(const CM2_Base_Instance* M2Instance, const UpdateEventArgs& e, float * rem, SGPUM2Particle * Particles) const
{
	float deltaTime = float(e.DeltaTime) / 1000.0f * float(e.DeltaTimeMultiplier);
	uint32 globalTime = static_cast<uint32>(e.TotalTime);

	uint32 sequence = M2Instance->getAnimator()->getSequenceIndex();
	uint32 sequenceTime = M2Instance->getAnimator()->getCurrentTime();

	glm::vec3 gravity;
	gravity = glm::vec3(0.0f, 0.0f, m_Gravity.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime));
	
	float deaccel = m_zSource.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);

	CreateAndDeleteParticles(M2Instance, e, rem, Particles);

	for (size_t i = 0; i < MAX_PARTICLES; i++)
	{
		SGPUM2Particle& p = Particles[i];
		if (false == p.Active)
			continue;

		p.speed += (p.down * gravity * deltaTime) - (p.dir * float(deaccel * deltaTime));

		float mspeed = 1.0f;

		if (m_M2Particle.drag != 0.0f)
			mspeed = glm::exp((-1.0f * m_M2Particle.drag) * p.currentTime);

		p.pos += (p.speed * float(mspeed * deltaTime));

		p.currentTime += deltaTime;
		float rlife = p.currentTime / p.maxTime;

		// calculate size and color based on lifetime
		p.size = ParticleSystemHelper::InterpolateParticleValue<float>(rlife, m_MiddleTime, m_Scales[0], m_Scales[1], m_Scales[2]);
		p.color = ParticleSystemHelper::InterpolateParticleValue<glm::vec4>(rlife, m_MiddleTime, m_Colors[0], m_Colors[1], m_Colors[2]);

		// kill off old Particles
		if (rlife >= 1.0f)
			p.Active = false;
	}
}

const IBlendState::BlendMode CM2_Part_ParticleSystem::GetBlendMode() const
{
	switch (m_M2Particle.blendingType)
	{
		case 0:
		{
			return IBlendState::BlendMode(false);
		}
		break;
		case 1:
		{
			return IBlendState::BlendMode(true, false, 
				IBlendState::BlendFactor::SrcColor, IBlendState::BlendFactor::One, 
				IBlendState::BlendOperation::Add, 
				IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::One
			);
		}
		break;
		case 2:
		{
			return IBlendState::BlendMode(true, false, 
				IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha, 
				IBlendState::BlendOperation::Add, 
				IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha
			);
		}
		break;
		case 3:
		{
			return IBlendState::BlendMode(false);
		}
		break;
		case 4:
		{
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::One,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::One
			);
		}
		break;
	}

	_ASSERT(false);
	return IBlendState::BlendMode();
}

const std::vector<SM2ParticleTileCoords>& CM2_Part_ParticleSystem::GetTiles() const
{
	return tiles;
}

void CM2_Part_ParticleSystem::CreateAndDeleteParticles(const CM2_Base_Instance * M2Instance, const UpdateEventArgs & e, float * rem, SGPUM2Particle * Particles) const
{
	double deltaTime = e.DeltaTime / 1000.0 * float(e.DeltaTimeMultiplier);
	uint32 globalTime = static_cast<uint32>(e.TotalTime);
	
	uint32 sequence = M2Instance->getAnimator()->getSequenceIndex();
	uint32 sequenceTime = M2Instance->getAnimator()->getCurrentTime();


	float emissionRateValue = emissionRate.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
	emissionRateValue += m_M2Particle.emissionRateVary * Random::Range(-1.0f, 1.0f);
	

	float lifeSpanValue = lifespan.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
	lifeSpanValue += m_M2Particle.lifespanVary * Random::Range(-1.0f, 1.0f);


	float ftospawn;

	if (lifeSpanValue != 0.0f)
		ftospawn = (deltaTime * emissionRateValue / lifeSpanValue) + *rem;
	else
		ftospawn = *rem;


	if (ftospawn < 1.0f)
	{
		*rem = ftospawn;
		if (*rem < 0)
			*rem = 0;
	}
	else
	{
		int tospawn = (int)ftospawn;
		*rem = ftospawn - (float)tospawn;

		float emissionAreaLengthValue = emissionAreaLength.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		float emissionAreaWidthValue = emissionAreaWidth.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		float emissionSpeedValue = emissionSpeed.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		float speedVariationValue = speedVariation.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		float lifespanValue = lifespan.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		float verticalRangeValue = verticalRange.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		float horizontalRangeValue = horizontalRange.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);

		bool enabledValue = true;
		if (enabled.IsUsesBySequence(sequence))
			enabledValue = enabled.GetValue(sequence, sequenceTime, m_M2Object.getSkeleton().getGlobalLoops(), globalTime) != 0;

		if (false == enabledValue)
			return;

		for (int i = 0; i < tospawn; i++)
		{
			size_t freeIndex = -1;
			for (size_t ind = 0; ind < MAX_PARTICLES; ind++)
			{
				if (false == Particles[ind].Active)
				{
					freeIndex = ind;
					break;
				}
			}

			if (freeIndex == -1)
			{
				*rem += tospawn - i - 1;
				break;
			}

			switch (m_M2Particle.emitterType)
			{
				case 0:
				case 3:
				{
					SGPUM2Particle p = DefaultGenerator_New(M2Instance, emissionAreaLengthValue, emissionAreaWidthValue, emissionSpeedValue, speedVariationValue, lifespanValue, verticalRangeValue, horizontalRangeValue);
					p.Active = true;
					Particles[freeIndex] = p;
				}
				break;
				case 1:
				{
					SGPUM2Particle p = PlaneGenerator_New(M2Instance, emissionAreaLengthValue, emissionAreaWidthValue, emissionSpeedValue, speedVariationValue, lifespanValue, verticalRangeValue, horizontalRangeValue);
					p.Active = true;
					Particles[freeIndex] = p;
				}
				break;
				case 2:
				{
					SGPUM2Particle p = SphereGenerator_New(M2Instance, emissionAreaLengthValue, emissionAreaWidthValue, emissionSpeedValue, speedVariationValue, lifespanValue, verticalRangeValue, horizontalRangeValue);
					p.Active = true;
					Particles[freeIndex] = p;
				}
				break;
				default:
					throw CException("Unknown emitter type");
			}
		}
	}
}

SGPUM2Particle CM2_Part_ParticleSystem::DefaultGenerator_New(const CM2_Base_Instance * M2Instance, float w, float l, float spd, float var, float lifespan, float spr, float spr2) const
{
	std::shared_ptr<CM2SkeletonBone3D> bone;
	if (GetBone() != -1)
		bone = M2Instance->getSkeletonComponent()->GetBone(GetBone());

	SGPUM2Particle p;
	p.pos = GetPosition();
	if (bone)
		p.pos = bone->GetMatrix() * glm::vec4(p.pos, 1.0f);

	p.dir = glm::vec3(0.0f, 1.0f, 0.0f);
	if (bone)
		p.dir = bone->GetRotateMatrix() * glm::vec4(p.dir, 0.0f);

	p.down = glm::vec3(0, -1.0f, 0);
	p.speed = glm::normalize(p.dir) * spd * (1.0f + Random::Range(-var, var));

	/*if (m_ParticleSystem->GetFlags().DONOTBILLBOARD)
	{
		p.corners[0] = mrot * vec4(-1, 0, +1, 0);
		p.corners[1] = mrot * vec4(+1, 0, +1, 0);
		p.corners[2] = mrot * vec4(+1, 0, -1, 0);
		p.corners[3] = mrot * vec4(-1, 0, -1, 0);
	}*/

	p.currentTime = 0;
	p.maxTime = lifespan;
	p.creationPoint = p.pos;
	p.tile = Random::Range(0, m_M2Particle.textureDimensions_rows * m_M2Particle.textureDimensions_columns - 1);
	return p;
}

SGPUM2Particle CM2_Part_ParticleSystem::PlaneGenerator_New(const CM2_Base_Instance * M2Instance, float w, float l, float spd, float var, float lifespan, float spr, float spr2) const
{
	std::shared_ptr<CM2SkeletonBone3D> bone;
	if (GetBone() != -1)
		bone = M2Instance->getSkeletonComponent()->GetBone(GetBone());

	SGPUM2Particle p;

	//glm::mat4 SpreadMat = CalcSpreadMatrix(spr, spr, 1.0f, 1.0f);
	//glm::mat4 mrot = bone->GetRotateMatrix() * SpreadMat;

	p.pos = GetPosition() + glm::vec3(Random::Range(-l, l), 0, Random::Range(-w, w));
	if (bone)
		p.pos = bone->GetMatrix() * glm::vec4(p.pos, 1.0f);

	p.dir = glm::vec3(0.0f, 1.0f, 0.0f);
	if (bone)
		p.dir = bone->GetRotateMatrix() * glm::vec4(p.dir, 0.0f);

	p.down = glm::vec3(0, -1.0f, 0);
	p.speed = glm::normalize(p.dir) * spd * (1.0f + Random::Range(-var, var));

	/*if (m_ParticleSystem->GetFlags().DONOTBILLBOARD)
	{
		p.corners[0] = mrot * vec4(-1, 0, +1, 0);
		p.corners[1] = mrot * vec4(+1, 0, +1, 0);
		p.corners[2] = mrot * vec4(+1, 0, -1, 0);
		p.corners[3] = mrot * vec4(-1, 0, -1, 0);
	}*/

	p.currentTime = 0;
	p.maxTime = lifespan;
	p.creationPoint = p.pos;
	p.tile = Random::Range(0, m_M2Particle.textureDimensions_rows * m_M2Particle.textureDimensions_columns - 1);
	return p;
}

SGPUM2Particle CM2_Part_ParticleSystem::SphereGenerator_New(const CM2_Base_Instance * M2Instance, float w, float l, float spd, float var, float lifespan, float spr, float spr2) const
{
	std::shared_ptr<CM2SkeletonBone3D> bone;
	if (GetBone() != -1)
		bone = M2Instance->getSkeletonComponent()->GetBone(GetBone());

	SGPUM2Particle p;
	glm::vec3 dir;

	float radius = Random::Range(0.0f, 1.0f);

	//Spread Calculation
	glm::mat4 SpreadMat = ParticleSystemHelper::CalcSpreadMatrix(spr * 2, spr2 * 2, w, l);
	glm::mat4 mrot = bone->GetRotateMatrix() * SpreadMat;

	glm::vec3 bdir = mrot * glm::vec4(glm::vec3(0, 1, 0) * radius, 0);
	float temp = bdir.z;
	bdir.z = bdir.y;
	bdir.y = temp;

	p.pos = GetPosition() + bdir;
	if (bone)
		p.pos = bone->GetMatrix() * glm::vec4(p.pos, 0.0f);


	/*if ((glm::length2(bdir) == 0) && ((m_ParticleSystem->flags & 0x100) != 0x100))
	{
		p.speed = vec3(0, 0, 0);
		dir = ParticleSystem_ParentBone->getRotateMatrix() * vec4(0, 1, 0, 0);
	}
	else
	{
		if (m_ParticleSystem->flags & 0x100)
			dir = ParticleSystem_ParentBone->getRotateMatrix() * vec4(0, 1, 0, 0);
		else
			dir = glm::normalize(bdir);

		p.speed = glm::normalize(dir) * spd * (1.0f + random.Range(-var, var));   // ?
	}*/

	if (bone)
		dir = bone->GetRotateMatrix() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	else
		dir = glm::normalize(bdir);

	p.speed = glm::normalize(dir) * spd * (1.0f + Random::Range(-var, var));   // ?
	p.dir = glm::normalize(dir);//mrot * vec3(0, 1.0f,0);
	p.down = glm::vec3(0, -1.0f, 0);
	p.currentTime = 0;
	p.maxTime = lifespan;
	p.creationPoint = p.pos;
	p.tile = glm::round(Random::Range(0, m_M2Particle.textureDimensions_rows * m_M2Particle.textureDimensions_columns - 1));
	return p;
}

#if 0

void CM2_Part_ParticleSystem::Render3D(const glm::mat4& _worldMatrix)
{
	/*switch (m_BlendType)
	{
		case 0:
		glDisable(GL_BLEND);
		//glDisable(GL_ALPHA_TEST);
		break;

		case 1:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_ONE);

		//glDisable(GL_ALPHA_TEST);
		break;

		case 2:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

		//glDisable(GL_ALPHA_TEST);
		break;

		case 3:
		glDisable(GL_BLEND);
		//glEnable(GL_ALPHA_TEST);
		break;

		case 4:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		//glDisable(GL_ALPHA_TEST);
		break;
	}*/

	// position stuff
	const float f = 1;//0.707106781f; // sqrt(2)/2
	vec3 bv0 = vec3(-f, +f, 0);
	vec3 bv1 = vec3(+f, +f, 0);
	vec3 bv2 = vec3(+f, -f, 0);
	vec3 bv3 = vec3(-f, -f, 0);

	/*if (billboard)
	{
		float modelview[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

		vRight = vec3(modelview[0], modelview[4], modelview[8]);
		vUp = vec3(modelview[1], modelview[5], modelview[9]);
	}*/

	/*
	if (billboard)
	{
		mat4 W = _worldMatrix;
		mat4 VW = _Render->getCamera()->getViewMat() * W;

		// Set vectors default
		vec3 worldScale = extractScale(W);
		vRight = vec3(VW[0][0], VW[1][0], VW[2][0]) / worldScale.x;
		vUp = vec3(VW[0][1], VW[1][1], VW[2][1]) / worldScale.y;
		//vec3 vForward = vec3(VW[0][2], VW[1][2], VW[2][2]) / worldScale.z;
		//vRight *= -1.0f;
	}
	*/

	/*
	* type:
	* 0	 "normal" particle
	* 1	large quad from the particle's creationPoint to its position (used in Moonwell water effects)
	* 2	seems to be the same as 0 (found some in the Deeprun Tram blinky-lights-sign thing)
	*/
	if (m_ParticleType == 0 || m_ParticleType == 2)
	{
		if (billboard)
		{
			for (auto& it : Particles)
			{
				vertices.push_back(ParticleVertex(it.pos - (vRight + vUp) * it.size, it.color, m_Tiles[it.m_TileExists].tc[0]));
				vertices.push_back(ParticleVertex(it.pos + (vRight - vUp) * it.size, it.color, m_Tiles[it.m_TileExists].tc[1]));
				vertices.push_back(ParticleVertex(it.pos + (vRight + vUp) * it.size, it.color, m_Tiles[it.m_TileExists].tc[2]));
				vertices.push_back(ParticleVertex(it.pos - (vRight - vUp) * it.size, it.color, m_Tiles[it.m_TileExists].tc[3]));

				m_Indices.push_back(cntr + 0);
				m_Indices.push_back(cntr + 2);
				m_Indices.push_back(cntr + 3);
				m_Indices.push_back(cntr + 0);
				m_Indices.push_back(cntr + 1);
				m_Indices.push_back(cntr + 2);

				cntr += 4;
			}

		}
		else
		{
			/*for (auto& it : Particles)
			{
				vertices.push_back(ParticleVertex(it.pos + it.corners[0] * it.size, it.color, m_Tiles[it.m_TileExists].tc[0]));
				vertices.push_back(ParticleVertex(it.pos + it.corners[1] * it.size, it.color, m_Tiles[it.m_TileExists].tc[1]));
				vertices.push_back(ParticleVertex(it.pos + it.corners[2] * it.size, it.color, m_Tiles[it.m_TileExists].tc[2]));
				vertices.push_back(ParticleVertex(it.pos + it.corners[3] * it.size, it.color, m_Tiles[it.m_TileExists].tc[3]));

				m_Indices.push_back(cntr + 0);
				m_Indices.push_back(cntr + 2);
				m_Indices.push_back(cntr + 3);
				m_Indices.push_back(cntr + 0);
				m_Indices.push_back(cntr + 1);
				m_Indices.push_back(cntr + 2);

				cntr += 4;
			}*/
		}
	}
	else if (m_ParticleType == 1)
	{ 
		for (auto& it : Particles)
		{
			vertices.push_back(ParticleVertex(it.pos + bv0 * it.size, it.color, m_Tiles[it.m_TileExists].tc[0]));
			vertices.push_back(ParticleVertex(it.pos + bv1 * it.size, it.color, m_Tiles[it.m_TileExists].tc[1]));
			vertices.push_back(ParticleVertex(it.creationPoint + bv1 * it.size, it.color, m_Tiles[it.m_TileExists].tc[2]));
			vertices.push_back(ParticleVertex(it.creationPoint + bv0 * it.size, it.color, m_Tiles[it.m_TileExists].tc[3]));

			m_Indices.push_back(cntr + 0);
			m_Indices.push_back(cntr + 2);
			m_Indices.push_back(cntr + 3);
			m_Indices.push_back(cntr + 0);
			m_Indices.push_back(cntr + 1);
			m_Indices.push_back(cntr + 2);

			cntr += 4;
		}
	}
}

#endif

void CM2_Part_ParticleSystem::InitTiles()
{
	// init tiles
	for (int i = 0; i < m_M2Particle.textureDimensions_rows * m_M2Particle.textureDimensions_columns; i++)
	{
		SM2ParticleTileCoords tcs;

		int x = i % m_M2Particle.textureDimensions_columns;
		int y = i / m_M2Particle.textureDimensions_columns;

		tcs.Start.x = x * (1.0f / m_M2Particle.textureDimensions_columns);
		tcs.Start.y = y * (1.0f / m_M2Particle.textureDimensions_rows);

		tcs.End.x = (x + 1) * (1.0f / m_M2Particle.textureDimensions_columns);
		tcs.End.y = (y + 1) * (1.0f / m_M2Particle.textureDimensions_rows);

		tiles.push_back(tcs);
	}
}
