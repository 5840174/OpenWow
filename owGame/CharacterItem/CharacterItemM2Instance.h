#pragma once

#ifdef USE_M2_MODELS

#include "../M2/M2_Base_Instance.h"

// FORWARD BEGIN
class CCharacter;
class CCharacterItem;
// FORWARD END

class CCharacterItemM2Instance
	: public CM2_Base_Instance
{
public:
	CCharacterItemM2Instance(IScene& Scene, CWorldClient& WorldClient, const std::shared_ptr<CM2>& M2Object, const CCharacterItem& CharacterItem, EM2_AttachmentPoint AttachmentPoint);
    virtual ~CCharacterItemM2Instance();

	// CCharacterItemM2Instance
	const CCharacterItem& GetCharacterItem() const;

	// ILoadable
	void OnAfterLoad() override;

private:
	const CCharacterItem& m_CharacterItem;
	EM2_AttachmentPoint m_AttachmentPoint;
	std::vector<std::shared_ptr<CM2_Base_Instance>> m_VisualEffects;
};

#endif