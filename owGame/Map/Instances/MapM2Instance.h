#pragma once

#ifdef USE_M2_MODELS

#include "M2/M2_Base_Instance.h"

struct ADT_MDXDef
{
	uint32 nameIndex;
	uint32 uniqueId;
	vec3 position;
	vec3 rotation;
	uint16 scale;
    struct
    {
        uint16 Flag_Biodome : 1;
        uint16 Flag_Shrubbery : 1;
        uint16 : 14;
    } flags;
};

class ZN_API CMapM2Instance 
	: public CM2_Base_Instance
{
public:
	CMapM2Instance(const std::shared_ptr<CM2>& M2Object, const ADT_MDXDef& _placementInfo);
	virtual ~CMapM2Instance();

	// ISceneNode
	void Initialize() override;
	void Accept(IVisitor* visitor) override;

protected:
	virtual void UpdateLocalTransform() override;

private: 
	const ADT_MDXDef m_PlacementInfo;

public:	// Static
	static void reset();
private:
	static std::unordered_map<uint32, const CMapM2Instance*> m_AlreadyDraw;
};

#endif