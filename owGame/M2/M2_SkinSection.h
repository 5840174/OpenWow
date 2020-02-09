#pragma once

#include "M2_CommonTypes.h"
#include "M2_SkinTypes.h"

// FORWARD BEGIN
class M2;
class CM2_Base_Instance;
class CM2_Skin_Builder;
// FORWARD END

class CM2_SkinSection
	: public GeometryProxie
{
public:
	CM2_SkinSection(IRenderDevice& RenderDevice, const M2& M2Model, const uint16 SkinSectionIndex, const SM2_SkinSection& SkinSectionProto, const std::vector<SM2_Vertex>& Vertexes, const std::vector<uint16>& Indexes);
	virtual ~CM2_SkinSection();

	void UpdateGeometryProps(const CM2_Base_Instance* M2Instance);
	const std::shared_ptr<IConstantBuffer>& GetGeometryPropsBuffer() const;

	uint16                  getIndex() const { return m_SkinSectionIndex; }
	const SM2_SkinSection&  getProto() const { return m_SkinSectionProto; }

private:
	const uint16            m_SkinSectionIndex;
	const SM2_SkinSection   m_SkinSectionProto;

private:
	__declspec(align(16)) struct ShaderM2GeometryProperties
	{
		ShaderM2GeometryProperties()
			: gIsAnimated(0)
			, gBonesMaxInfluences(0)
		{}

		uint32 gIsAnimated;
		uint32 gBonesMaxInfluences;
		glm::vec2 __padding;

		glm::mat4 Bones[200];
	};
	ShaderM2GeometryProperties* m_Properties;

	std::shared_ptr<IConstantBuffer> m_PropertiesBuffer;

private:
	IRenderDevice&          m_RenderDevice;
	const M2&               m_M2Model;
};