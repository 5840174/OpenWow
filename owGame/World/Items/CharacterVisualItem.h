#pragma once

#ifdef USE_M2_MODELS

#include "../MeshIDEnums.h"
#include "../Character/Types/CharacterTemplate.h"

#include "Item_M2Instance.h"

// FORWARD BEGIN
class Character;
// FORWARD END

struct ObjectComponent
{
	std::shared_ptr<CItem_M2Instance> model;
	std::shared_ptr<IImage>           texture;
	EM2_AttachmentType	              attach;
};

struct GeosetComponent
{
	MeshIDType        mesh;
	uint32            value;

	uint32            getMeshID() const { return 1 + value; }
};

class CCharacterVisualItem 
	: public CLoadableObject
{
public:
	CCharacterVisualItem(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<Character>& OwnerCharacter);
	virtual ~CCharacterVisualItem();
	
	CInet_ItemTemplate&                  Template()                                                  { return m_Template; }
	const CInet_ItemTemplate&            GetTemplate() const                                         { return m_Template; }

	const std::vector<ObjectComponent>&  getObjectComponents()                                 const { return m_ObjectComponents; }
	const std::vector<GeosetComponent>&  getGeosetComponents()                                 const { return m_GeosetComponents; }
	const std::shared_ptr<IImage>&       getTextureComponent(DBC_CharComponent_Sections _type) const { return m_TextureComponents[static_cast<size_t>(_type)]; }

	// CLoadableObject
	bool Load();
	bool Delete();

private:
	void InitObjectComponents();
	void InitGeosetComponents();
	void InitTextureComponents();

	std::string				GetObjectModelName(DBCItem_EInventoryItemSlot _objectType, std::string _modelName);
	std::shared_ptr<IImage> LoadObjectTexture (DBCItem_EInventoryItemSlot _objectType, std::string _textureName);
	std::shared_ptr<IImage> LoadSkinTexture   (DBC_CharComponent_Sections _type, std::string _textureName);

private:
	CInet_ItemTemplate			  m_Template;
	std::vector<ObjectComponent>  m_ObjectComponents;
	std::vector<GeosetComponent>  m_GeosetComponents;
	std::shared_ptr<IImage>       m_TextureComponents[static_cast<size_t>(DBC_CharComponent_Sections::ITEMS_COUNT)];
	
private: 
	IBaseManager&                 m_BaseManager;
	IRenderDevice&                m_RenderDevice;
	Character&                    m_OwnerCharacter;
};

#endif