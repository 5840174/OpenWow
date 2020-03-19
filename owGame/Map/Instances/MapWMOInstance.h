#pragma once

#include "WMO/WMO_Base_Instance.h"

struct ADT_MODF
{
	uint32 nameIndex;		// Ref to MWID
	uint32 uniqueId;		// this ID should be unique for all ADTs currently loaded. Best, they are unique for the whole map.
	glm::vec3 position;
	glm::vec3 rotation;
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

class ZN_API CMapWMOInstance 
	: public CWMO_Base_Instance
{
public:
	CMapWMOInstance(const std::shared_ptr<CWMO>& WMOObject, const ADT_MODF& _placementInfo);
	virtual ~CMapWMOInstance();

	// CWMO_Base_Instance
	uint16 GetDoodadSetIndex() const override;

	// ISceneNode
	void Initialize() override;
	void Accept(IVisitor* visitor) override;

protected:
	virtual glm::mat4 CalculateLocalTransform() const override;
	
private:
	const ADT_MODF m_PlacementInfo;

public:	// Static
	static void reset();
private:
	static std::unordered_map<uint32, const CMapWMOInstance*> m_AlreadyDraw;
};