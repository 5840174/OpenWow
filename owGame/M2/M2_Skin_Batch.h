#pragma once

#include "M2_Part_Material.h"
#include "M2_SkinSection.h"
#include "M2_Material.h"

// FORWARD BEGIN
class M2;
class CM2_Base_Instance;
class CM2_Skin_Builder;
// FORWARD END

class CM2_Skin_Batch 
	: public ModelProxie
{
	friend CM2_Skin_Builder;
public:
	CM2_Skin_Batch(IBaseManager* BaseManager, IRenderDevice& RenderDevice, const M2& M2Model, const CM2_SkinSection& SkinSection);
	virtual ~CM2_Skin_Batch();

	// IModel
	bool Render(const RenderEventArgs& renderEventArgs) const override final;

	// IVisitorAcceptable
	void Accept(IVisitor* visitor) override final;

public:
	int32 getPriorityPlan()  const { return m_PriorityPlan; }
	const CM2_SkinSection&  getSkin() const { return m_SkinSection; }

public:
	int32												m_PriorityPlan;
	
	// Material props
	std::shared_ptr<const CM2_Part_Color>				m_Color;
	std::shared_ptr<const CM2_Part_Material>			m_M2ModelMaterial;
	std::vector<std::weak_ptr<const CM2_Part_Texture>>	m_Textures;
	int16												m_TextureUnit;
	std::shared_ptr<const CM2_Part_TextureWeight>		m_TextureWeight;
	std::shared_ptr<const CM2_Part_TextureTransform>	m_TextureTransform;

	int32												newShader;
	std::shared_ptr<M2_Material>                        m_TestMaterial;


private:
	IBaseManager* m_BaseManager;
	IRenderDevice& m_RenderDevice;
	const M2& m_M2Model;
	const CM2_SkinSection& m_SkinSection;
};
