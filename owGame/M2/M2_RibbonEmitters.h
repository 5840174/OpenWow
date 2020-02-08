#pragma once

// FORWARD BEGIN
class CM2_Part_Bone;
// FORWARD END

struct RibbonSegment
{
	vec3 pos, up, back;
	float len, len0;
};

class CM2_RibbonEmitters
{
public:
	CM2_RibbonEmitters(const std::weak_ptr<M2> _model, std::shared_ptr<IFile> f, const SM2_RibbonEmitter& mta, cGlobalLoopSeq globals);

	void setup(uint16 anim, uint32 time, uint32 _globalTime, cmat4 _worldMatrix);

	void Render(cmat4 _world);

private:
	const std::weak_ptr<M2>						m_ParentM2;

	uint32										m_ID;
	std::weak_ptr<const CM2_Part_Bone>			m_Bone;
	vec3										pos;
	vec3										posValue;

	std::shared_ptr<ITexture>					m_Texture;
	std::weak_ptr<const CM2_Part_Material>		m_Material;

	//std::shared_ptr<IBuffer>								__vb;
	//std::shared_ptr<Mesh>								__geom;

	M2_Animated<vec3>							m_Color;
	M2_Animated<float, short, ShortToFloat>		m_Alpha;
	M2_Animated<float>							m_HeightAbove;
	M2_Animated<float>							m_HeightBelow;
	
	vec4										tcolor;
	float										tabove;
	float										tbelow;
	
	float length;
	float m_EdgesLifeTime;
	int m_EdgesPerSecond;

	std::list<RibbonSegment> segs;
};