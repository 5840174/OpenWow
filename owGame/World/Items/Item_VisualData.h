#pragma once

#ifdef USE_M2_MODELS

#include "World/MeshIDEnums.h"
#include "Client/Templates/CharacterEnums.h"
#include "Client/Templates/ItemTemplate.h"

#include "Item_M2Instance.h"

// FORWARD BEGIN
class Character;
// FORWARD END

struct ObjectComponent
{
	std::shared_ptr<CItem_M2Instance> model;
	std::shared_ptr<ITexture>         texture;
	M2_AttachmentType	              attach;
};

struct GeosetComponent
{
	MeshIDType::List  mesh;
	uint32            value;
	uint32 getMeshID() const { return 1 + value; }
};

class CItem_VisualData 
	: public CInet_ItemTemplate
	, public CLoadableObject
{
public:
	CItem_VisualData(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<Character>& Character);
	virtual ~CItem_VisualData();

	// CLoadableObject
	bool Load();
	bool Delete();

	const std::vector<ObjectComponent>&  getObjectComponents() const { return m_ObjectComponents; }
	const std::vector<GeosetComponent>&  getGeosetComponents() const { return m_GeosetComponents; }
	const std::shared_ptr<ITexture>&     getTextureComponent(DBC_CharComponent_Sections _type) const { return m_TextureComponents[static_cast<size_t>(_type)]; }

private:
	void InitObjectComponents();
	void InitGeosetComponents();
	void InitTextureComponents();

	std::string				 GetObjectModelName(EInventoryType _objectType, std::string _modelName);
	//std::shared_ptr<M2>      LoadObjectModel   (EInventoryType::List _objectType, std::string _modelName);
	std::shared_ptr<ITexture> LoadObjectTexture (EInventoryType _objectType, std::string _textureName);
	std::shared_ptr<ITexture> LoadSkinTexture   (DBC_CharComponent_Sections _type, std::string _textureName);
	
	// Helpers
	std::string getTextureComponentName(DBC_CharComponent_Sections _type, std::string _textureName, Gender _gender);
	char getGenderLetter(Gender _gender);

private:
	std::vector<ObjectComponent>  m_ObjectComponents;
	std::vector<GeosetComponent>  m_GeosetComponents;
	std::shared_ptr<ITexture>     m_TextureComponents[static_cast<size_t>(DBC_CharComponent_Sections::ITEMS_COUNT)];
	
private: 
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	const CDBCStorage* m_DBCs;
	Character& m_ParentCharacter;
};

#endif