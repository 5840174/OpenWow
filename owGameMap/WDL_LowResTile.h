#pragma once

// FORWARD BEGIN
class MapController;
// FORWARD END

class CWDL_LowResTile : public MeshWrapper
{
public:
	CWDL_LowResTile(std::weak_ptr<const MapController> _parent, std::shared_ptr<IMesh> _mesh, uint32 _indexX, uint32 _indexZ);

	// IMesh
	bool Render(RenderEventArgs& renderEventArgs, std::shared_ptr<ConstantBuffer> perObject, UINT indexStartLocation, UINT indexCnt, UINT vertexStartLocation, UINT vertexCnt) override;

private:
	const int m_IndexX, m_IndexZ;

	// PARENT
	const std::weak_ptr<const MapController> m_MapController;
};