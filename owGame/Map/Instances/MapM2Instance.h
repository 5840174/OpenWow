#pragma once

#ifdef USE_M2_MODELS

#include "../M2/M2_Base_Instance.h"
#include "../Map_Headers.h"

class ZN_API CMapM2Instance 
	: public CM2_Base_Instance
{
public:
	CMapM2Instance(IScene& Scene, const std::shared_ptr<CM2>& M2Object, const SMapTile_MDDF& MDDF);
	virtual ~CMapM2Instance();

	// ISceneNode
	void Initialize() override;
	void Accept(IVisitor* visitor) override;

	// ILoadable
	bool Load() override;

protected:
	virtual glm::mat4 CalculateLocalTransform() const override;

private:
	const SMapTile_MDDF m_PlacementInfo;

public:	// Static
	static void reset();
private:
	static std::map<uint32, const CMapM2Instance*> m_AlreadyDraw;
};

#endif