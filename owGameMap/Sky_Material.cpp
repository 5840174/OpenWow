#include "stdafx.h"

// General
#include "Sky_Material.h"

Sky_Material::Sky_Material(std::shared_ptr<IRenderDevice> RenderDevice) 
	: MaterialProxie(RenderDevice->CreateMaterial(0))
{

}

Sky_Material::~Sky_Material()
{
}
