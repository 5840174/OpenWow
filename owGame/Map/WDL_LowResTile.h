#pragma once

// FORWARD BEGIN
class CMap;
// FORWARD END

class CWDL_LowResTile 
	: public ModelProxie
{
public:
	CWDL_LowResTile(IRenderDevice& RenderDevice, const CMap& Map, int IndexX, int IndexZ);
	virtual ~CWDL_LowResTile();

	// IModel
	bool Render(const RenderEventArgs& renderEventArgs) const override;

private:
	const CMap& m_MapController;

	const int m_IndexX;
	const int m_IndexZ;
};