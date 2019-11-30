#pragma once

#include "MeshIDEnums.h"
#include "CharacterEnums.h"

#include "ItemTemplate.h"
#include "Item_M2Instance.h"

// FORWARD BEGIN
class Character;
class Creature_M2Instance;
// FORWARD END

struct ObjectComponent
{
	std::shared_ptr<CItem_M2Instance>   model;
	std::shared_ptr<ITexture>            texture;
	std::shared_ptr<const CM2_Part_Attachment>	attach;
};

struct GeosetComponent
{
	MeshIDType::List  mesh;
	uint32            value;
	uint32 getMeshID() const { return 1 + value; }
};

class CItem_VisualData : public ItemTemplate
{
public:
	CItem_VisualData(std::shared_ptr<Character> _owner);

	void Load();

	const std::vector<ObjectComponent>&  getObjectComponents() const { return m_ObjectComponents; }
	const std::vector<GeosetComponent>&  getGeosetComponents() const { return m_GeosetComponents; }
	const std::shared_ptr<ITexture>&      getTextureComponent(DBC_CharComponent_Sections::List _type) const { return m_TextureComponents[_type]; }

	void Render3D();

private:
	void InitObjectComponents();
	void InitGeosetComponents();
	void InitTextureComponents();

	std::string				 GetObjectModelName(InventoryType::List _objectType, std::string _modelName);
	//std::shared_ptr<M2>      LoadObjectModel   (InventoryType::List _objectType, std::string _modelName);
	std::shared_ptr<ITexture> LoadObjectTexture (InventoryType::List _objectType, std::string _textureName);
	std::shared_ptr<ITexture> LoadSkinTexture   (DBC_CharComponent_Sections::List _type, std::string _textureName);
	
	// Helpers
	std::string getTextureComponentName(DBC_CharComponent_Sections::List _type, std::string _textureName, Gender::List _gender);
	char getGenderLetter(Gender::List _gender);

private:
	std::vector<ObjectComponent>  m_ObjectComponents;
	std::vector<GeosetComponent>  m_GeosetComponents;
	std::shared_ptr<ITexture>      m_TextureComponents[DBC_CharComponent_Sections::ITEMS_COUNT];
	
private: // PARENT
	std::weak_ptr<Character>      m_ParentCharacter;
};