#pragma once

#ifdef USE_M2_MODELS

#include "../MeshIDEnums.h"
#include "../Character/Types/CharacterTemplate.h"

#include "CharacterItemDefines.h"
#include "CharacterItemM2Instance.h"

// FORWARD BEGIN
class CCharacter;
// FORWARD END

struct SModelComponent
{
	std::shared_ptr<CCharacterItemM2Instance> ItemM2Instance;
	std::shared_ptr<IImage>                   ItemSelfTexture;
};

struct SGeosetComponent
{
	EM2GeosetType mesh;
	uint32        value;

	uint32        getMeshID() const { return 1 + value; }
};

class CCharacterItem 
	: public CLoadableObject
	, public std::enable_shared_from_this<CCharacterItem>
{
public:
	CCharacterItem(const IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<CCharacter>& OwnerCharacter, const SCharacterItemTemplate& CharacterItemTemplate);
	virtual ~CCharacterItem();
	
	const SCharacterItemTemplate&        GetTemplate()                                    const { return m_Template; }
	bool                                 IsExists()                                       const { return (GetTemplate().DisplayId != 0) && (GetTemplate().InventoryType != (uint8)DBCItem_EInventoryItemType::NON_EQUIP); }

	const std::vector<SModelComponent>&  GetModels()                                      const { return m_Models; }
	const std::vector<SGeosetComponent>& GetGeosets()                                     const { return m_Geosets; }
	const std::shared_ptr<IImage>&       GetSkinComponentImage(DBC_CharComponent_Sections SkinComponent) const { return m_SkinComponentImages[static_cast<size_t>(SkinComponent)]; }

	bool                                 IsNeedHideHelmetGeoset(EM2GeosetType M2GeosetType) const;

	// ILoadable
	bool Load() override;
	bool Delete() override;
	void OnAfterLoad() override;

private:
	void InitializeItemModels();
	void InitializeItemGeosets();
	void InitializeItemSkinImages();

	std::shared_ptr<CM2>	LoadItemM2Model            (DBCItem_EInventoryItemType ObjectType, std::string ModelFileName);
	std::shared_ptr<IImage> LoadItemImage              (DBCItem_EInventoryItemType ObjectType, std::string TextureFileName);
	std::shared_ptr<IImage> LoadItemSkinComponentImage (DBC_CharComponent_Sections SkinComponent, std::string TextureFileName);

private:
	SCharacterItemTemplate	         m_Template;
	const DBC_ItemDisplayInfoRecord* m_ItemDisplayInfoRecord;
	const DBC_HelmetGeosetVisDataRecord * m_HelmetGeosetVisDataRecord;

	std::vector<SModelComponent> m_Models;
	std::vector<SGeosetComponent>  m_Geosets;
	std::shared_ptr<IImage>       m_SkinComponentImages[static_cast<size_t>(DBC_CharComponent_Sections::ITEMS_COUNT)];
	
private: 
	const IBaseManager&           m_BaseManager;
	IRenderDevice&                m_RenderDevice;
	CCharacter&                   m_OwnerCharacter;
};

#endif