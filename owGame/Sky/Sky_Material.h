#pragma once

class Sky_Material 
	: public MaterialProxie
{
public:
	Sky_Material(IRenderDevice& RenderDevice);
	virtual ~Sky_Material();
};