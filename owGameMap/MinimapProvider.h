#pragma once

struct MinimapData_Map
{
	uint8		X_Coord;
	uint8		Y_Coord;
	std::string		filename;
};

struct MinimapDir
{
	MinimapDir()
	{
	}

	void Load();
	void Render(cvec3 _pos);

	std::string						name;
	std::vector<MinimapData_Map*>	data;
	std::shared_ptr<ITexture>        textures[C_TilesInMap][C_TilesInMap];
};

class CMinimapProvider
{
public:
	CMinimapProvider();
	~CMinimapProvider();

	//--

	void Init();
	MinimapDir* getMinimap(std::string _name);

private:
	std::shared_ptr<IFile> m_File;

	std::vector<MinimapDir*> m_Minimaps;
	const CGroupVideo* m_GroupVideo;
};