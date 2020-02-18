#pragma once

#include "Client/Templates/CharacterEnums.h"
#include "Client/Templates/ItemTemplate.h"

#include "World/MeshIDEnums.h"


class Item 
	: public CInet_ItemTemplate
{
public:
	Item();

	void Init(CInet_ItemTemplate& _template);

private:

public:
	std::string					gender;
};