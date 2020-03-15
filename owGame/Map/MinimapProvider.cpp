#include "stdafx.h"

// General
#include "MinimapProvider.h"


glm::vec2 C_MinimapSize = glm::vec2(600.0f, 600.0f);

CMinimapProvider::CMinimapProvider(std::shared_ptr<IRenderDevice> RenderDevice)
{
	m_File = RenderDevice->GetBaseManager().GetManager<IFilesManager>()->Open("textures\\Minimap\\md5translate.trs");
}

CMinimapProvider::~CMinimapProvider()
{}

void CMinimapProvider::Init()
{
	MinimapDir* currentDir = nullptr;

	while (!m_File->isEof())
	{
		std::string curStr;
		m_File->readLine(&curStr);

		if (curStr.substr(0, 4) == "dir:")
		{
			if (currentDir != nullptr)
			{
				m_Minimaps.push_back(currentDir);
			}

			currentDir = new MinimapDir;
			currentDir->name = curStr.substr(5);

			continue;
		}

		if (currentDir->name.find("WMO\\") != -1)
		{
			continue;
		}

		// Add new block
		{
			MinimapData_Map* minimapData = new MinimapData_Map;
			int tabIndex = curStr.find('\t');
			_ASSERT(tabIndex != -1);

			std::string beforeTab = curStr.substr(0, tabIndex);
			sscanf_s
			(
				(beforeTab.c_str()), 
				(currentDir->name + "\\map%d_%d.blp").c_str(), 
				&(minimapData->X_Coord), 
				&(minimapData->Y_Coord)
			);
			minimapData->filename = curStr.substr(tabIndex + 1);

			currentDir->data.push_back(minimapData);
		}
	}
}

MinimapDir* CMinimapProvider::getMinimap(std::string _name)
{
	for (auto& it : m_Minimaps)
	{
		if (it->name == _name)
		{
			return it;
		}
	}
	
	return nullptr;
}

void MinimapDir::Load()
{
	/*for (auto& it : data)
	{
		textures[it->X_Coord][it->Y_Coord] = _RenderDevice->CreateTexture2D("textures\\Minimap\\" + it->filename);
		delete it;
		it = nullptr;
	}
	data.clear();*/
}

void MinimapDir::Render(const glm::vec3& _pos)
{
	/*int8 _x = _pos.x / C_TileSize;
	int8 _z = _pos.z / C_TileSize;

	vec2 startPos = vec2(1280 - C_MinimapSize.x, 0);
	vec2 oneTextureSize = C_MinimapSize / 3.0f;

	float convertKoeff = oneTextureSize.x / C_TileSize;
	vec2 posInMinimap = vec2(_pos.x - _x * C_TileSize, _pos.z - _z * C_TileSize) * convertKoeff + oneTextureSize;
	vec2 correctPoint = posInMinimap - C_MinimapSize / 2.0f;


	_Render->r.setScissorTest(true);

	_Render->r.setScissorRect
	(
		startPos.x + oneTextureSize.x,
		1024 - oneTextureSize.y * 2,
		oneTextureSize.x,
		oneTextureSize.y
	);
	

	for (uint8 i = 0; i < 3; i++)
	{
		for (uint8 j = 0; j < 3; j++)
		{
			int8 XC = i + _x - 1 ;
			int8 ZC = j + _z - 1;
			
			if (textures[XC][ZC] == nullptr)
			{
				continue;
			}

			if (XC < 0 || XC >= C_TilesInMap ||	ZC < 0 || ZC >= C_TilesInMap)
			{
				continue;
			}

			_Render->RenderTexture
			(
				startPos + vec2(i * oneTextureSize.x, j * oneTextureSize.y) - correctPoint,
				textures[XC][ZC],
				oneTextureSize
				//,				degToRad(_Render->getCamera()->Roll)
			);
		}
	}
	_Render->RenderRectangle(startPos + oneTextureSize * 1.5f, vec2(5.0f, 5.0f));


	_Render->r.setScissorTest(false);*/
}
