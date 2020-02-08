#include "stdafx.h"

// General
#include "WMOsManager.h"

WMOsManager::WMOsManager(IBaseManager* BaseManager)
	: m_BaseManager(BaseManager)
{
}

WMOsManager::~WMOsManager()
{
}

std::shared_ptr<CWMO> WMOsManager::CreateAction(IRenderDevice& RenderDevice, const std::string& name)
{
	std::shared_ptr<CWMO> _wmo = std::make_shared<CWMO>(m_BaseManager, RenderDevice, name);
	LoadAction(name, _wmo);
	return _wmo;
}

void WMOsManager::LoadAction(std::string name, std::shared_ptr<CWMO>& item)
{
	if (!item->Load())
	{
		Log::Error("WMOsManager[%s]: Failed init WMO.", name.c_str());
	}
}

bool WMOsManager::DeleteAction(const std::string& name)
{
	return true;
}

void WMOsManager::MakeContext()
{

}
