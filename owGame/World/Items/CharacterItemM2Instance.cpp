#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "../Character/Character.h"
#include "CharacterItem.h"

// General
#include "CharacterItemM2Instance.h"

// Additional
#include "CharacterItemEffectM2Instance.h"

CCharacterItemM2Instance::CCharacterItemM2Instance(IScene& Scene, const std::shared_ptr<CM2>& M2Object, const CCharacterItem& CharacterItem, EM2_AttachmentPoint AttachmentPoint)
	: CM2_Base_Instance(Scene, M2Object)
	, m_CharacterItem(CharacterItem)
	, m_AttachmentPoint(AttachmentPoint)
{}

CCharacterItemM2Instance::~CCharacterItemM2Instance()
{}



//
// CCharacterItemM2Instance
//
const CCharacter& CCharacterItemM2Instance::GetCharacterInstance() const 
{ 
	auto parentIsCharacter = GetParent();
	if (parentIsCharacter == nullptr)
		throw CException("CCharacterItemM2Instance::GetCharacterInstance: M2Parent must exists.");

	if (auto loadable = std::dynamic_pointer_cast<ILoadable>(parentIsCharacter))
	{
		auto state = loadable->GetState();
		if (state != ILoadable::ELoadableState::Loaded)
			throw CException("CCharacterItemM2Instance::GetCharacterInstance: M2Parent Character isn't loaded. State = '%d'.", state);
	}

	return dynamic_cast<CCharacter&>(*parentIsCharacter);
}

const CCharacterItem& CCharacterItemM2Instance::GetCharacterItem() const 
{ 
	if (auto loadable = dynamic_cast<const ILoadable*>(&m_CharacterItem))
	{
		auto state = loadable->GetState();
		if (state != ILoadable::ELoadableState::Loaded)
			throw CException("CCharacterItemM2Instance::GetCharacterItem: CharacterItem isn't loaded. State = '%d'.", state);
	}

	return m_CharacterItem; 
}

//void CCharacterItemM2Instance::AddVisualEffect(std::shared_ptr<CM2_Base_Instance> _visualEffect)
//{
//	m_VisualEffects.push_back(_visualEffect);
//}



//
// ILoadable
//
void CCharacterItemM2Instance::OnLoaded()
{
	// Attach to parent (parent is Character)
	if (auto ownerCharacterAttachment = GetCharacterInstance().getM2().getMiscellaneous().getAttachment(m_AttachmentPoint))
		Attach(ownerCharacterAttachment->GetAttachmentType());

	// Item enchants
	const DBC_ItemVisualsRecord* itemVisualsRecord = GetBaseManager().GetManager<CDBCStorage>()->DBC_ItemVisuals()[GetCharacterItem().GetTemplate().EnchantAuraID];
	if (itemVisualsRecord != nullptr)
	{
		for (uint32 itemAttachmentPoint = 0; itemAttachmentPoint < DBC_ItemVisuals_VisualEffect_Count; itemAttachmentPoint++)
		{
			uint32 itemVisualEffectID = itemVisualsRecord->Get_VisualEffect(itemAttachmentPoint);
			const DBC_ItemVisualEffectsRecord* itemVisualEffectRecord = GetBaseManager().GetManager<CDBCStorage>()->DBC_ItemVisualEffects()[itemVisualEffectID];
			if (itemVisualEffectRecord == nullptr)
				continue;

			std::string itemVisualEffectRecordModelFilename = itemVisualEffectRecord->Get_Name();
			if (itemVisualEffectRecordModelFilename.empty())
				continue;

			// M2 model
			auto itemVisualEffectModel = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadM2(GetRenderDevice(), itemVisualEffectRecordModelFilename);
			if (itemVisualEffectModel == nullptr)
				throw CException("CCharacterItem::InitializeItemModels: itemVisualEffectM2Model is nullptr. ItemVisualsRecord ID = '%d', ItemVisualEffectsRecord = '%d' Index = '%d', Path '%s'.", GetCharacterItem().GetTemplate().EnchantAuraID, itemVisualEffectID, itemAttachmentPoint, itemVisualEffectRecordModelFilename.c_str());

			// M2 Instance
			auto itemVisualEffectInstance = CreateSceneNode<CCharacterItemEffectM2Instance>(itemVisualEffectModel, m_CharacterItem);
			AddChildLoadable(itemVisualEffectInstance);
			GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(itemVisualEffectInstance);

			auto itemModelAttachment = getM2().getMiscellaneous().getAttachment((EM2_AttachmentPoint)itemAttachmentPoint);
			if (itemModelAttachment != nullptr)
			{
				itemVisualEffectInstance->Attach(itemModelAttachment->GetAttachmentType()); // Attach visual effect to itemInstace attachment
			}
			else
			{
				//itemVisualEffectInstance->Attach(ownerCharacterAttachmentPoint); // Or attach visual effect to same attachment as item
			}

			//AddVisualEffect(itemVisualEffectInstance);
		}
	}
}

#endif