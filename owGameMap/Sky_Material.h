#pragma once

class Sky_Material : public MaterialProxie
{
public:
	Sky_Material(std::shared_ptr<IRenderDevice> RenderDevice);
	virtual ~Sky_Material();
};