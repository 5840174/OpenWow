#pragma once

// FORWARD BEGIN
class CWMO;
class M2;
// FORWARD END

struct
	__declspec(novtable, uuid("42D47100-B825-47F1-BE2F-6F7C78443884"))
	IWMOManager : public IRefManager<CWMO>
{
	virtual ~IWMOManager() {};
};

struct
	__declspec(novtable, uuid("B14D922C-BE9E-44CA-9448-5400E3CB573A"))
	IM2Manager : public IRefManager<M2>
{
	virtual ~IM2Manager() {};
};