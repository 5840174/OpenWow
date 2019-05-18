#pragma once

#include "M2_Part_Material.h"
#include "M2_SkinSection.h"
#include "M2_Material.h"

// FORWARD BEGIN
class M2;
class CM2_Base_Instance;
class CM2_Skin_Builder;
// FORWARD END

class CM2_Skin_Batch : public MeshWrapper
{
	friend CM2_Skin_Builder;
public:
	CM2_Skin_Batch(const std::weak_ptr<const M2> _parentM2, std::shared_ptr<IMesh> _mesh);

	void PostInit();

	// IMesh
	bool Render(const RenderEventArgs* renderEventArgs, std::shared_ptr<ConstantBuffer> perObject, UINT indexStartLocation = 0, UINT indexCnt = 0, UINT vertexStartLocation = 0, UINT vertexCnt = 0) override;

	int32                                 getPriorityPlan()  const { return m_PriorityPlan; }
	std::weak_ptr<const CM2_SkinSection>  getSkin()          const { return m_SkinSection; }

	std::shared_ptr<const CM2_Part_Material> GetM2Material() const { return m_Material; }

private:
	int32												m_PriorityPlan;
	std::shared_ptr<CM2_SkinSection>					m_SkinSection;

	// Material props
	std::shared_ptr<const CM2_Part_Color>				m_Color;
	std::shared_ptr<const CM2_Part_Material>			m_Material;
	std::vector<std::weak_ptr<const CM2_Part_Texture>>	m_Textures;
	int16												m_TextureUnit;
	std::shared_ptr<const CM2_Part_TextureWeight>		m_TextureWeight;
	std::shared_ptr<const CM2_Part_TextureTransform>	m_TextureTransform;

	int32												newShader;
	std::shared_ptr<M2_Material>                        m_TestMaterial;

private: // Parent
	const std::weak_ptr<const M2> m_ParentM2;
	const CGroupQuality&          m_QualitySettings;
};
