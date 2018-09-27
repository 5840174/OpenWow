#include "stdafx.h"

// Inlcudes
#include "M2.h"

// General
#include "ParticleSystem.h"

CM2_ParticleSystem::CM2_ParticleSystem(M2* _parentM2, IFile* f, const SM2_Particle& _proto, cGlobalLoopSeq globals) :
	m_ParentM2(_parentM2),
	m_Emitter(nullptr), 
	mid(0), 
	rem(0),
	m_CurrentAnimation(0),
	m_CurrentTime(0.0),
	m_GlobalTime(0.0)
{
	m_Position = Fix_XZmY(_proto.Position);
	m_ParentBone = m_ParentM2->getSkeleton()->getBoneDirect(_proto.bone);

	texture = m_ParentM2->getMaterials()->m_Textures[_proto.texture]->getTexture();

	blend = _proto.blendingType;
	uint8 emitterType = _proto.emitterType;
	type = _proto.particleColorIndex;
	order = _proto.particleColorIndex > 0 ? -1 : 0; //order = _proto.s2;

	// uint8 particleType;                       // Found below.
	// uint8 headorTail;                         // 0 - Head, 1 - Tail, 2 - Both 

	// int16 textureTileRotation
	rows = _proto.textureDimensions_rows;
	cols = _proto.textureDimensions_columns;

	emissionSpeed.init(_proto.emissionSpeed, f, globals);
	speedVariation.init(_proto.speedVariation, f, globals);
	verticalRange.init(_proto.verticalRange, f, globals);
	horizontalRange.init(_proto.horizontalRange, f, globals);
	gravity.init(_proto.gravity, f, globals);
	lifespan.init(_proto.lifespan, f, globals);
	emissionRate.init(_proto.emissionRate, f, globals);
	emissionAreaLength.init(_proto.emissionAreaLength, f, globals);
	emissionAreaWidth.init(_proto.emissionAreaWidth, f, globals);
	zSource.init(_proto.zSource, f, globals);
	enabled.init(_proto.enabledIn, f, globals);

	// Blend state
	m_State.setBlendMode(true, R_BlendFunc::BS_BLEND_SRC_ALPHA, R_BlendFunc::BS_BLEND_ONE);
	m_State.setCullMode(R_CullMode::RS_CULL_BACK);
	m_State.setDepthMask(false);
	_ASSERT(texture != nullptr);
	m_State.setTexture(Material::C_DiffuseTextureIndex, texture, 0, 0);


	vec3 colors2[3];
	memcpy(colors2, f->getData() + _proto.colorTrack.values.offset, sizeof(vec3) * 3);

	for (uint32 i = 0; i < 3; i++)
	{
		float opacity = *(short*)(f->getData() + _proto.alphaTrack.values.offset + i * 2);

		colors[i] = vec4(colors2[i].x / 255.0f, colors2[i].y / 255.0f, colors2[i].z / 255.0f, opacity / 32767.0f);
		sizes[i] = (*(float*)(f->getData() + _proto.scaleTrack.values.offset + i * 4)) * _proto.scales[i];
	}

	mid = 0.5;
	slowdown = _proto.slowdown; // FIXME
	rotation = _proto.rotation; // FIXME
	
	switch (emitterType)
	{
	case 1:
		m_Emitter = new PlaneParticleEmitter(this);
		break;
	case 2:
		m_Emitter = new SphereParticleEmitter(this);
		break;
	case 3: // Spline? (can't be bothered to find one)
		break;
	}

	//transform = _proto.flags & 1024;

	// Type 2
	// 3145 = water ele
	// 1305 = water ele
	// 1049 = water elemental
	// 1033 = water elemental
	// 281 = water ele
	// 256 = Water elemental
	// 57 = Faith halo, ring?
	// 9 = water elemental

	billboard = !(_proto.flags.DONOTBILLBOARD);

	// init tiles
	for (int i = 0; i < rows*cols; i++)
	{
		TexCoordSet tc;
		initTile(tc.tc, i);
		m_Tiles.push_back(tc);
	}
}

CM2_ParticleSystem::~CM2_ParticleSystem()
{
	delete m_Emitter;
}

void CM2_ParticleSystem::update(double _time, double _dTime)
{
	_dTime /= 1000.0;

	float grav     = gravity.getValue(m_CurrentAnimation, m_CurrentTime, m_GlobalTime);
	float deaccel  = zSource.getValue(m_CurrentAnimation, m_CurrentTime, m_GlobalTime);

	// spawn new particles
	if (m_Emitter)
	{
		float frate = emissionRate.getValue(m_CurrentAnimation, m_CurrentTime, m_GlobalTime);
		float flife = lifespan.getValue(m_CurrentAnimation, m_CurrentTime, m_GlobalTime);

		float ftospawn = (_dTime * frate / flife) + rem;
		if (ftospawn < 1.0f)
		{
			rem = ftospawn;
			if (rem < 0)
				rem = 0;
		}
		else
		{
			int tospawn = (int)ftospawn;

			if ((tospawn + particles.size()) > MAX_PARTICLES) // Error check to prevent the program from trying to load insane amounts of particles.
				tospawn = (int)particles.size() - MAX_PARTICLES;

			rem = ftospawn - (float)tospawn;


			float emissionAreaLengthValue = emissionAreaLength.getValue(m_CurrentAnimation, m_CurrentTime, m_GlobalTime) * 0.5f;
			float emissionAreaWidthValue = emissionAreaWidth.getValue(m_CurrentAnimation, m_CurrentTime, m_GlobalTime) * 0.5f;
			float emissionSpeedValue = emissionSpeed.getValue(m_CurrentAnimation, m_CurrentTime, m_GlobalTime);
			float speedVariationValue = speedVariation.getValue(m_CurrentAnimation, m_CurrentTime, m_GlobalTime);
			float verticalRangeValue = verticalRange.getValue(m_CurrentAnimation, m_CurrentTime, m_GlobalTime);
			float horizontalRangeValue = horizontalRange.getValue(m_CurrentAnimation, m_CurrentTime, m_GlobalTime);
			
			bool enabledValue = true;
			if (enabled.uses(m_CurrentAnimation))
			{
				enabledValue = enabled.getValue(m_CurrentAnimation, m_CurrentTime, m_GlobalTime) != 0;
			}

			if (enabledValue)
			{
				for (int i = 0; i < tospawn; i++)
				{
					Particle p = m_Emitter->newParticle(m_CurrentAnimation, m_CurrentTime, emissionAreaLengthValue, emissionAreaWidthValue, emissionSpeedValue, speedVariationValue, verticalRangeValue, horizontalRangeValue, m_GlobalTime);
					particles.push_back(p);
				}
			}
		}
	}

	float mspeed = 1.0f;

	for (auto& it = particles.begin(); it != particles.end(); )
	{
		Particle &p = *it;
		p.speed += (p.down * float(grav * _dTime)) - (p.dir * float(deaccel * _dTime));

		if (slowdown > 0)
		{
			mspeed = expf(-1.0f * slowdown * p.life);
		}
		p.pos += (p.speed * float(mspeed * _dTime));

		p.life += _dTime;
		float rlife = p.life / p.maxlife;

		// calculate size and color based on lifetime
		p.size = lifeRamp<float>(rlife, mid, sizes[0], sizes[1], sizes[2]);
		p.color = lifeRamp<vec4>(rlife, mid, colors[0], colors[1], colors[2]);

		// kill off old particles
		if (rlife >= 1.0f)
			particles.erase(it++);
		else
			++it;
	}
}

void CM2_ParticleSystem::setup(uint16 anim, uint32 time, uint32 _globalTime)
{
	m_CurrentAnimation = anim;
	m_CurrentTime = time;
	m_GlobalTime = _globalTime;
}

struct ParticleVertex
{
	ParticleVertex(vec3 _pos, vec4 _color, vec2 _tex) :
		pos(_pos),
		color(_color),
		tex(_tex)
	{}

	vec3 pos;
	vec4 color;
	vec2 tex;
};

void CM2_ParticleSystem::Render3D(cmat4 _worldMatrix)
{
	if (particles.empty())
	{
		return;
	}

	/*switch (blend)
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

	//glDisable(GL_CULL_FACE);
	//glDepthMask(GL_FALSE);

	vec3 vRight(1, 0, 0);
	vec3 vUp(0, 1, 0);

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

	vector<ParticleVertex> vertices;
	vector<uint16> m_Indices;
	uint32 cntr = 0;

	/*
	* type:
	* 0	 "normal" particle
	* 1	large quad from the particle's origin to its position (used in Moonwell water effects)
	* 2	seems to be the same as 0 (found some in the Deeprun Tram blinky-lights-sign thing)
	*/
	if (type == 0 || type == 2)
	{
		if (billboard)
		{
			for (auto& it : particles)
			{
				if (m_Tiles.size() - 1 < it.m_TileExists) break;

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
			for (auto& it : particles)
			{
				if (m_Tiles.size() - 1 < it.m_TileExists) break;

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
			}
		}
	}
	else if (type == 1)
	{ 
		for (auto& it : particles)
		{
			if (m_Tiles.size() - 1 < it.m_TileExists) break;

			vertices.push_back(ParticleVertex(it.pos + bv0 * it.size, it.color, m_Tiles[it.m_TileExists].tc[0]));
			vertices.push_back(ParticleVertex(it.pos + bv1 * it.size, it.color, m_Tiles[it.m_TileExists].tc[1]));
			vertices.push_back(ParticleVertex(it.origin + bv1 * it.size, it.color, m_Tiles[it.m_TileExists].tc[2]));
			vertices.push_back(ParticleVertex(it.origin + bv0 * it.size, it.color, m_Tiles[it.m_TileExists].tc[3]));

			m_Indices.push_back(cntr + 0);
			m_Indices.push_back(cntr + 2);
			m_Indices.push_back(cntr + 3);
			m_Indices.push_back(cntr + 0);
			m_Indices.push_back(cntr + 1);
			m_Indices.push_back(cntr + 2);

			cntr += 4;
		}
	}

	if (vertices.empty())
	{
		return;
	}

	// Vertex buffer
	_ASSERT(vertices.data() != nullptr);
	SharedBufferPtr __vb = _Render->r.createVertexBuffer(vertices.size() * sizeof(ParticleVertex), vertices.data(), false);

	// Index buffer
	_ASSERT(m_Indices.data() != nullptr);
	SharedBufferPtr __ib = _Render->r.createIndexBuffer(m_Indices.size() * sizeof(uint16), m_Indices.data(), false);

	// Geometry
	SharedMeshPtr __geom = _Render->r.beginCreatingGeometry(PRIM_TRILIST, _Render->getRenderStorage()->__layout_GxVBF_PCT);
	__geom->setGeomVertexParams(__vb, R_DataType::T_FLOAT, 0, sizeof(ParticleVertex));
	__geom->setGeomVertexParams(__vb, R_DataType::T_FLOAT, 12, sizeof(ParticleVertex));
	__geom->setGeomVertexParams(__vb, R_DataType::T_FLOAT, 28, sizeof(ParticleVertex));
	__geom->setGeomIndexParams(__ib, R_IndexFormat::IDXFMT_16);
	__geom->finishCreatingGeometry();

	m_State.setGeometry(__geom);

	CM2_Particle_Pass* pass = _Render->getTechniquesMgr()->M2_Particles_Pass.operator->();
	pass->Bind();
	{
		pass->setWorld(_worldMatrix);



		_Render->r.drawIndexed(0, m_Indices.size(), 0, vertices.size(), &m_State, true);
	}
	pass->Unbind();
}

//

void CM2_ParticleSystem::initTile(vec2 *tc, int num)
{
	
	vec2 a, b;
	int x = num % cols;
	int y = num / cols;

	a.x = x * (1.0f / cols);
	a.y = y * (1.0f / rows);

	b.x = (x + 1) * (1.0f / cols);
	b.y = (y + 1) * (1.0f / rows);

	vec2 otc[4];
	otc[0] = a;
	otc[1].x = b.x;
	otc[1].y = a.y;
	otc[2] = b;
	otc[3].x = a.x;
	otc[3].y = b.y;

	for (int i = 0; i < 4; i++)
	{
		tc[(i + 4 - order) & 3] = otc[i];
	}
}