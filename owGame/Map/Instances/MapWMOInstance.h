#pragma once

#include "WMO/WMO_Base_Instance.h"

struct ADT_MODF
{
	uint32 nameIndex;		// Ref to MWID
	uint32 uniqueId;		// this ID should be unique for all ADTs currently loaded. Best, they are unique for the whole map.
	vec3 position;		
	vec3 rotation;
	CAaBox boundingBox;		// position plus the transformed wmo bounding box. used for defining if they are rendered as well as collision.
	struct Flags
	{
		uint16 Destroyable : 1;
		uint16 : 15;
	} flags;
	uint16 doodadSetIndex;	// which WMO doodad set is used.
	uint16 nameSet;			// which WMO name set is used. Used for renaming goldshire inn to northshire inn while using the same model.
	uint16 scale;			// Legion+
};

class CMapWMOInstance 
	: public CWMO_Base_Instance
{
public:
	CMapWMOInstance(const CWMO& WMOObject, const ADT_MODF& _placementInfo);
	virtual ~CMapWMOInstance();

	// ISceneNode
	void Accept(IVisitor* visitor) override;

	void DoUpdate(UpdateEventArgs& e) override;

private:
	uint32									m_UniqueId;

public:	// Static
	static void reset();
private:
	static std::set<uint32> m_AlreadyDraw;
};