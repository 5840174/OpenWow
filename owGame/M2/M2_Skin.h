#pragma once

#include "M2_SkinTypes.h"

#include "M2_Skin_Batch.h"
#include "M2_SkinSection.h"

// FORWARD BEGIN
class M2;
class CM2_Base_Instance;
class CM2_Skin_Builder;
// FORWARD END

class CM2_Skin 
	: public ModelProxie
{
	friend CM2_Skin_Builder;
public:
	CM2_Skin(IRenderDevice& RenderDevice, const M2& M2Model);
	virtual ~CM2_Skin();

	const std::unordered_map<std::shared_ptr<CM2_SkinSection>, std::vector<std::shared_ptr<CM2_Skin_Batch>>>& GetTTT() const
	{
		return m_TTT;
	}

	// IModel
	void Accept(IVisitor* visitor) override final;

private:
	std::vector<std::shared_ptr<CM2_SkinSection>> m_Sections; // 'Geometries'
	std::vector<std::shared_ptr<CM2_Skin_Batch>> m_Batches;   // 'Materials'

	std::unordered_map<std::shared_ptr<CM2_SkinSection>, std::vector<std::shared_ptr<CM2_Skin_Batch>>> m_TTT;
	
private: // PARENT
	IRenderDevice& m_RenderDevice;
	const M2& m_M2Model;
};