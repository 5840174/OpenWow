#include "stdafx.h"

// General
#include "Sky_Material.h"

Sky_Material::Sky_Material(IRenderDevice& RenderDevice) 
	: MaterialProxie(RenderDevice.GetObjectsFactory().CreateMaterial(0))
{
	SetWrapper(this);
}

Sky_Material::~Sky_Material()
{}
