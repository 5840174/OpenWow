#pragma once

#include "WMO.h"

class WMOsManager 
	: public IWMOManager
	, public CRefManager1Dim<CWMO>
{
public:
	WMOsManager(IBaseManager* BaseManager);
    virtual ~WMOsManager();

	// IWMOManager
	std::shared_ptr<CWMO> Add(const std::string& name) { return CRefManager1Dim::Add(name); }
	bool Exists(const std::string& name) const { return CRefManager1Dim::Exists(name); }
	void Delete(const std::string& name) { CRefManager1Dim::Delete(name); }
	void Delete(std::shared_ptr<CWMO> item) { CRefManager1Dim::Delete(item); }

	// CRefManager1Dim
	std::shared_ptr<CWMO> CreateAction(const std::string& name) override;
	void LoadAction(std::string name, std::shared_ptr<CWMO>& item);
	bool DeleteAction(const std::string& name) override;
	void MakeContext();

private:
	IBaseManager* m_BaseManager;
};