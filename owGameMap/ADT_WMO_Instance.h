#pragma once

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

class ADT_WMO_Instance : public CWMO_Base_Instance
{
public:
	ADT_WMO_Instance(std::weak_ptr<SceneNode> _parent, SmartWMOPtr _wmoObject, ADT_MODF& _placementInfo);
	virtual ~ADT_WMO_Instance();

	void Load();

	// IRenderable3D
	bool PreRender3D();
	void Render3D();

private:
	uint32									m_UniqueId;

public:	// Static
	static void reset();
private:
	static std::set<uint32> m_AlreadyDraw;
};