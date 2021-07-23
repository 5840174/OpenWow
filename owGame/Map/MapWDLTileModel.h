#pragma once

// FORWARD BEGIN
class CMap;
// FORWARD END

class CMapWDLTileModel 
	: public ModelProxie
{
public:
	CMapWDLTileModel(IRenderDevice& RenderDevice, const CMap& Map, int32 IndexX, int32 IndexZ);
	virtual ~CMapWDLTileModel();

	bool IsNeedRender() const;

private:
	const CMap& m_Map;
	const int32 m_IndexX;
	const int32 m_IndexZ;
};