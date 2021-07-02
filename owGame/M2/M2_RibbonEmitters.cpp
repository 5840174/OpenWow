#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Part_Bone.h"

// General
#include "M2_RibbonEmitters.h"

struct RibbonVertex
{
	RibbonVertex(glm::vec3 _pos, glm::vec2 _tex) :
		pos(_pos),
		tex(_tex)
	{}

	glm::vec3 pos;
	glm::vec2 tex;
};

CM2_RibbonEmitters::CM2_RibbonEmitters(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_RibbonEmitter& M2RibbonEmitter) 
	: m_M2Object(M2Object)
	, m_M2RibbonEmitter(M2RibbonEmitter)
	, tcolor(glm::vec4(1.0f))
{
	_ASSERT_EXPR(false, L"TODO!");
	// TODOm_Bone = m_M2Object.getSkeleton().GetBones()[M2RibbonEmitter.boneIndex];
	posValue = pos = Fix_XZmY(M2RibbonEmitter.position);

	m_Color.Initialize(M2RibbonEmitter.colorTrack, File, M2Object.getSkeleton().GetAnimFiles());
	m_Alpha.Initialize(M2RibbonEmitter.alphaTrack, File, M2Object.getSkeleton().GetAnimFiles());
	m_HeightAbove.Initialize(M2RibbonEmitter.heightAboveTrack, File, M2Object.getSkeleton().GetAnimFiles());
	m_HeightBelow.Initialize(M2RibbonEmitter.heightBelowTrack, File, M2Object.getSkeleton().GetAnimFiles());

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
	length = M2RibbonEmitter.edgesPerSecond * M2RibbonEmitter.edgeLifetime;

	// create first segment
	SM2RibbonSegment rs;
	rs.pos = posValue;
	rs.len = 0;
	m_Segments.push_back(rs);
}

void CM2_RibbonEmitters::setup(uint16 anim, uint32 time, uint32 _globalTime, const glm::mat4& _worldMatrix)
{
	std::shared_ptr<const CM2_Part_Bone> Bone = m_Bone.lock();
	_ASSERT(Bone != nullptr);

	_ASSERT_EXPR(false, L"TODO!");
	glm::vec3 ntpos = _worldMatrix * (/*TODO: Bone->getTransformMatrix() **/ glm::vec4(pos, 0));
	glm::vec3 ntup = _worldMatrix * (/*TODO: Bone->getTransformMatrix() **/ glm::vec4((pos + glm::vec3(0, 0, 1.0f)), 0));

	ntup -= ntpos;
	ntup = glm::normalize(ntup);
	float dlen = glm::length(ntpos - posValue);

	// move first segment
	SM2RibbonSegment& first = *m_Segments.begin();
	if (first.len > m_M2RibbonEmitter.edgeLifetime)
	{
		// add new segment
		first.back = glm::normalize(posValue - ntpos);
		first.len0 = first.len;

		SM2RibbonSegment newseg;
		newseg.pos = ntpos;
		newseg.up = ntup;
		newseg.len = dlen;
		m_Segments.push_front(newseg);
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
	for (auto it = m_Segments.begin(); it != m_Segments.end(); )
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
			m_Segments.erase(it++);
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

void CM2_RibbonEmitters::Render(const glm::mat4& _world)
{
	std::vector<RibbonVertex> vertices;

	float l = 0;
	for (const auto& segsIt : m_Segments)
	{
		float u = l / length;
		vertices.push_back(RibbonVertex(segsIt.pos + segsIt.up * tabove, glm::vec2(u, 0)));
		vertices.push_back(RibbonVertex(segsIt.pos - segsIt.up * tbelow, glm::vec2(u, 1)));

		l += segsIt.len;
	}

	if (m_Segments.size() > 1)
	{
		const auto& lastSegment = m_Segments.back();
		vertices.push_back(RibbonVertex(lastSegment.pos + lastSegment.up * tabove + lastSegment.back * (lastSegment.len / lastSegment.len0), glm::vec2(1, 0)));
		vertices.push_back(RibbonVertex(lastSegment.pos - lastSegment.up * tbelow + lastSegment.back * (lastSegment.len / lastSegment.len0), glm::vec2(1, 1)));
	}



	/*texture->Bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	glColor4fv(tcolor);

	glBegin(GL_QUAD_STRIP);
	{
		list<SM2RibbonSegment>::iterator it = m_Segments.begin();
		float l = 0;
		for (; it != m_Segments.end(); ++it)
		{
			float u = l / length;

			glTexCoord2f(u, 0);
			glVertex3fv(it->pos + it->up * tabove);

			glTexCoord2f(u, 1);
			glVertex3fv(it->pos - it->up * tbelow);

			l += it->len;
		}

		if (m_Segments.size() > 1)
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
