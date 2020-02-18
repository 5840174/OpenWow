#pragma once

#include "WMO.h"

class WMOsManager 
	: public IWMOManager
	, public CRefManager1Dim<CWMO>
{
public:
	WMOsManager(IBaseManager& BaseManager);
    virtual ~WMOsManager();

	// IWMOManager
	std::shared_ptr<CWMO> Add(IRenderDevice& RenderDevice, const std::string& name) override { return CRefManager1Dim::Add(RenderDevice, name); }
	bool Exists(const std::string& name) const override { return CRefManager1Dim::Exists(name); }
	void Delete(const std::string& name) override { CRefManager1Dim::Delete(name); }
	void Delete(std::shared_ptr<CWMO> item) override { CRefManager1Dim::Delete(item); }

	// CRefManager1Dim
	std::shared_ptr<CWMO> CreateAction(IRenderDevice& RenderDevice, const std::string& name) override;
	bool DeleteAction(const std::string& name) override;
	void MakeContext();

private:
	IBaseManager& m_BaseManager;
};