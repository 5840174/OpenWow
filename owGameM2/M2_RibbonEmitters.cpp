#include "stdafx.h"

// Include
#include "M2.h"

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

CM2_RibbonEmitters::CM2_RibbonEmitters(const std::weak_ptr<M2> _model, std::shared_ptr<IFile> f, const SM2_RibbonEmitter& _proto, cGlobalLoopSeq globals) :
	m_ParentM2(_model),
	tcolor(vec4(1.0f))
{
	const std::shared_ptr<M2> ParentM2 = m_ParentM2.lock();
	_ASSERT(ParentM2 != nullptr);

	m_Bone = (ParentM2->getSkeleton()->getBoneDirect(_proto.boneIndex));
	posValue = pos = Fix_XZmY(_proto.position);

	m_Color.init(_proto.colorTrack, f, globals);
	m_Alpha.init(_proto.alphaTrack, f, globals);
	m_HeightAbove.init(_proto.heightAboveTrack, f, globals);
	m_HeightBelow.init(_proto.heightBelowTrack, f, globals);

	{
		uint16_t* TexturesList = (uint16_t*)(f->getData() + _proto.textureIndices.offset);
		// just use the first texture for now; most models I've checked only had one
		_ASSERT(_proto.textureIndices.size > 0);
		m_Texture = ParentM2->getMaterials()->m_Textures[TexturesList[0]]->getTexture();

		uint16_t* MaterialsList = (uint16_t*)(f->getData() + _proto.materialIndices.offset);
		_ASSERT(_proto.materialIndices.size > 0);
		m_Material = (ParentM2->getMaterials()->GetMaterial(MaterialsList[0]));
	}

	// TODO: figure out actual correct way to calculate length
	// in BFD, res is 60 and len is 0.6, the trails are very short (too long here)
	// in CoT, res and len are like 10 but the trails are supposed to be much longer (too short here)
	m_EdgesPerSecond = (int)_proto.edgesPerSecond;
	m_EdgesLifeTime = _proto.edgeLifetime;
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
	if (m_Color.uses(anim) && m_Alpha.uses(anim))
	{
		tcolor = vec4(m_Color.getValue(anim, time, _globalTime), m_Alpha.getValue(anim, time, _globalTime));
	}

	if (m_HeightAbove.uses(anim))
	{
		tabove = m_HeightAbove.getValue(anim, time, _globalTime);
	}

	if (m_HeightBelow.uses(anim))
	{
		tbelow = m_HeightBelow.getValue(anim, time, _globalTime);
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