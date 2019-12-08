#pragma once

class CMapShared
{
public:
	CMapShared(std::shared_ptr<IRenderDevice> RenderDevice);
	virtual ~CMapShared();

	std::shared_ptr<IBuffer> BufferTextureCoordDetail;
	std::shared_ptr<IBuffer> BufferTextureCoordAlpha;

	static std::string getMapFolder(std::shared_ptr<DBC_MapRecord> _map);

	std::vector<uint16> GenarateHighMapArray(uint16 _holes = 0);
	std::vector<uint16> GenarateDefaultMapArray(uint16 _holes = 0);

private:
	std::vector<uint16>  m_HighMapStrip;
	std::vector<uint16>  m_DefaultMapStrip;
};

extern CMapShared* _MapShared;
