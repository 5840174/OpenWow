#pragma once

// FORWARD BEGIN
class CMap;
// FORWARD END

class CWDL_LowResTile 
	: public MeshProxie
{
public:
	CWDL_LowResTile(std::weak_ptr<const CMap> _parent, std::shared_ptr<IMesh> _mesh, uint32 _indexX, uint32 _indexZ);

	// IMesh
	bool Render(const RenderEventArgs* renderEventArgs, const IConstantBuffer* perObject, SGeometryPartParams GeometryPartParams = SGeometryPartParams()) override;

private:
	const int m_IndexX, m_IndexZ;

	// PARENT
	const std::weak_ptr<const CMap> m_MapController;
};