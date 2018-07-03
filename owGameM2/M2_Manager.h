#pragma once

#include "M2.h"

class CM2_Manager : public IM2Manager, public CRefManager1Dim<M2>
{
public:
	CM2_Manager();
	~CM2_Manager();

	// IM2Manager
	M2* Add(cstring name) { return CRefManager1Dim::Add(name); }
	bool Exists(cstring name) const { return CRefManager1Dim::Exists(name); }
	void Delete(cstring name) { CRefManager1Dim::Delete(name); }
	void Delete(M2* item) { CRefManager1Dim::Delete(item); }

	// CRefManager1Dim
	M2* CreateAction(cstring name) override;
	bool DeleteAction(cstring name) override;

private:
	M2 * m_DefaultModel;
};
