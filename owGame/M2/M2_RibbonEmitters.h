#pragma once

// FORWARD BEGIN
class CM2;
class CM2_Part_Bone;
// FORWARD END

struct SM2RibbonSegment
{
	glm::vec3 pos, up, back;
	float len, len0;
};

class CM2_Part_RibbonEmitters
{
public:
	CM2_Part_RibbonEmitters(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_RibbonEmitter& M2RibbonEmitter);

	void setup(uint16 anim, uint32 time, uint32 _globalTime, const glm::mat4& _worldMatrix);

	void Render(const glm::mat4& _world);

private:
	std::weak_ptr<const CM2_Part_Bone>	        m_Bone;
	glm::vec3									pos;
	glm::vec3									posValue;

	std::shared_ptr<ITexture>					m_Texture;
	std::weak_ptr<const CM2_Part_Material>		m_Material;

	M2_Animated<glm::vec3>						m_Color;
	M2_Animated<float, short, ShortToFloat>		m_Alpha;
	M2_Animated<float>							m_HeightAbove;
	M2_Animated<float>							m_HeightBelow;
	
	glm::vec4									tcolor;
	float										tabove;
	float										tbelow;
	
	float                                       length;

	std::list<SM2RibbonSegment>                 m_Segments;

private:
	const CM2&                                  m_M2Object;
	const SM2_RibbonEmitter                     m_M2RibbonEmitter;
};
