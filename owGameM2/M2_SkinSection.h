#pragma once

#include "M2_CommonTypes.h"
#include "M2_SkinTypes.h"

// FORWARD BEGIN
class M2;
class CM2_Base_Instance;
class CM2_Skin_Builder;
// FORWARD END

class CM2_SkinSection
{
	friend class CM2_Skin_Builder;
public:
	CM2_SkinSection(IBaseManager* BaseManager, const std::weak_ptr<const M2> M2Model, const uint16 _index, const SM2_SkinSection& _proto, const std::vector<SM2_Vertex>& _vertexes, const std::vector<uint16>& _indexes);

	uint16                        getIndex() const { return m_Index; }
	const SM2_SkinSection&        getProto() const { return m_Proto; }
	const std::shared_ptr<IMesh>  getMesh()  const { return m_Mesh; }

private:
	const uint16           m_Index;
	const SM2_SkinSection  m_Proto;
	std::shared_ptr<IMesh> m_Mesh;

private:
	const std::weak_ptr<const M2> m_ParentM2;
};