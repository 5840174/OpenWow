#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Part_Bone.h"

// General
#include "M2_RibbonEmitters.h"

struct RibbonVertex
{
	RibbonVertex(vec3 _pos, vec2 _tex) :
		pos(_pos),
		tex(_tex)
	{}

	vec3 pos;
	vec2 tex;
};

CM2_RibbonEmitters::CM2_RibbonEmitters(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_RibbonEmitter& M2RibbonEmitter) 
	: m_M2Object(M2Object)
	, tcolor(vec4(1.0f))
{
	m_Bone = m_M2Object.getSkeleton().getBoneDirect(M2RibbonEmitter.boneIndex);
	posValue = pos = Fix_XZmY(M2RibbonEmitter.position);

	m_Color.Initialize(M2RibbonEmitter.colorTrack, File);
	m_Alpha.Initialize(M2RibbonEmitter.alphaTrack, File);
	m_HeightAbove.Initialize(M2RibbonEmitter.heightAboveTrack, File);
	m_HeightBelow.Initialize(M2RibbonEmitter.heightBelowTrack, File);

	{
		uint16_t* TexturesList = (uint16_t*)(File->getData() + M2RibbonEmitter.textureIndices.offset);
		// just use the first texture for now; most models I've checked only had one
		_ASSERT(M2RibbonEmitter.textureIndices.size > 0);
		m_Texture = m_M2Object.getMaterials().GetTextureDirectInternal(TexturesList[0])->GetTexture();

		uint16_t* MaterialsList = (uint16_t*)(File->getData() + M2RibbonEmitter.materialIndices.offset);
		_ASSERT(M2RibbonEmitter.materialIndices.size > 0);
		m_Material = m_M2Object.getMaterials().GetMaterialDirect(MaterialsList[0]);
	}

	// TODO: figure out actual correct way to calculate length
	// in BFD, res is 60 and len is 0.6, the trails are very short (too long here)
	// in CoT, res and len are like 10 but the trails are supposed to be much longer (too short here)
	m_EdgesPerSecond = (int)M2RibbonEmitter.edgesPerSecond;
	m_EdgesLifeTime = M2RibbonEmitter.edgeLifetime;
	length = (float)m_EdgesPerSecond * m_EdgesLifeTime;

	// create first segment
	RibbonSegment rs;
	rs.pos = posValue;
	rs.len = 0;
	segs.push_back(rs);
}

void CM2_RibbonEmitters::setup(uint16 anim, uint32 time, uint32 _globalTime, cmat4 _worldMatrix)
{
	std::shared_ptr<const CM2_Part_Bone> Bone = m_Bone.lock();
	_ASSERT(Bone != nullptr);

	vec3 ntpos = _worldMatrix * (Bone->getTransformMatrix() * vec4(pos, 0));
	vec3 ntup = _worldMatrix * (Bone->getTransformMatrix() * vec4((pos + vec3(0, 0, 1.0f)), 0));

	ntup -= ntpos;
	ntup = glm::normalize(ntup);
	float dlen = glm::length(ntpos - posValue);

	// move first segment
	RibbonSegment& first = *segs.begin();
	if (first.len > m_EdgesLifeTime)
	{
		// add new segment
		first.back = glm::normalize(posValue - ntpos);
		first.len0 = first.len;

		RibbonSegment newseg;
		newseg.pos = ntpos;
		newseg.up = ntup;
		newseg.len = dlen;
		segs.push_front(newseg);
	}
	else
	{
		first.up = ntup;
		first.pos = ntpos;
		first.len += dlen;
	}

	// kill stuff from the end
	float l = 0;
	bool erasemode = false;
	for (auto it = segs.begin(); it != segs.end(); )
	{
		if (!erasemode)
		{
			l += it->len;
			if (l > length)
			{
				it->len = l - length;
				erasemode = true;
			}
			++it;
		}
		else
		{
			segs.erase(it++);
		}
	}

	posValue = ntpos;
	if (m_Color.IsUsesBySequence(anim) && m_Alpha.IsUsesBySequence(anim))
	{
		tcolor = glm::vec4(m_Color.GetValue(anim, time, m_M2Object.getSkeleton().getGlobalLoops(), _globalTime), m_Alpha.GetValue(anim, time, m_M2Object.getSkeleton().getGlobalLoops(), _globalTime));
	}

	if (m_HeightAbove.IsUsesBySequence(anim))
	{
		tabove = m_HeightAbove.GetValue(anim, time, m_M2Object.getSkeleton().getGlobalLoops(), _globalTime);
	}

	if (m_HeightBelow.IsUsesBySequence(anim))
	{
		tbelow = m_HeightBelow.GetValue(anim, time, m_M2Object.getSkeleton().getGlobalLoops(), _globalTime);
	}
}

void CM2_RibbonEmitters::Render(cmat4 _world)
{
	std::vector<RibbonVertex> vertices;

	std::list<RibbonSegment>::iterator it = segs.begin();
	float l = 0;
	for (; it != segs.end(); ++it)
	{
		float u = l / length;

		vertices.push_back(RibbonVertex(it->pos + it->up * tabove, vec2(u, 0)));
		vertices.push_back(RibbonVertex(it->pos - it->up * tbelow, vec2(u, 1)));

		l += it->len;
	}

	if (segs.size() > 1)
	{
		// last segment...?
		--it;
		vertices.push_back(RibbonVertex(it->pos + it->up * tabove + it->back*(it->len / it->len0), vec2(1, 0)));
		vertices.push_back(RibbonVertex(it->pos - it->up * tbelow + it->back*(it->len / it->len0), vec2(1, 1)));
	}



	/*texture->Bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	glColor4fv(tcolor);

	glBegin(GL_QUAD_STRIP);
	{
		list<RibbonSegment>::iterator it = segs.begin();
		float l = 0;
		for (; it != segs.end(); ++it)
		{
			float u = l / length;

			glTexCoord2f(u, 0);
			glVertex3fv(it->pos + it->up * tabove);

			glTexCoord2f(u, 1);
			glVertex3fv(it->pos - it->up * tbelow);

			l += it->len;
		}

		if (segs.size() > 1)
		{
			// last segment...?
			--it;
			glTexCoord2f(1, 0);
			glVertex3fv(it->pos + it->up * tabove + it->back*(it->len / it->len0));

			glTexCoord2f(1, 1);
			glVertex3fv(it->pos - it->up * tbelow + it->back*(it->len / it->len0));
		}
	}
	glEnd();

	glColor4f(1, 1, 1, 1);
	glEnable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);*/


	for (auto& it : vertices)
	{
		//_Render->DrawSphere(mat4(), it.pos, 0.1f);
	}


	// Vertex buffer
	/*std::shared_ptr<IBuffer> __vb = _Render->r.createVertexBuffer(vertices.size() * sizeof(RibbonVertex), vertices.data());

	// Geometry
	std::shared_ptr<IModel> __geom = _Render->r.beginCreatingGeometry(PRIM_TRISTRIP, _Render->getRenderStorage()->__layout_GxVBF_PT);
	__geom->setGeomVertexParams(__vb, R_DataType::T_FLOAT, 0, sizeof(RibbonVertex));
	__geom->setGeomVertexParams(__vb, R_DataType::T_FLOAT, 12, sizeof(RibbonVertex));
	__geom->finishCreatingGeometry();

	//__vb->updateBufferData(0, vertices.size() * sizeof(RibbonVertex), vertices.data());

	_Render->r.setBlendMode(true, R_BlendFunc::BS_BLEND_SRC_ALPHA, R_BlendFunc::BS_BLEND_ONE);
	_Render->r.setCullMode(R_CullMode::RS_CULL_NONE);
	_Render->r.setDepthMask(false);

	_Render->r.setGeometry(__geom);

	CM2_RibbonEmitters_Pass* pass = _Render->getTechniquesMgr()->M2_RibbonEmitters_Pass.operator->();
	pass->Bind();
	{
		pass->setWorld(mat4());
		pass->SetColor(vec4(1.0f));

		_Render->r.setTexture(Material::C_DiffuseTextureIndex, m_Texture, 0, 0);

		_Render->r.draw(0, vertices.size());
	}
	pass->Unbind();*/
}