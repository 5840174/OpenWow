#pragma once

// FORWARD BEGIN
class CMap;
// FORWARD END

class CMapWDLTileModel 
	: public ModelProxie
{
public:
	CMapWDLTileModel(IRenderDevice& RenderDevice, const CMap& Map, int IndexX, int IndexZ);
	virtual ~CMapWDLTileModel();

	bool IsNeedRender() const;

private:
	const CMap& m_Map;
	const int m_IndexX;
	const int m_IndexZ;
};