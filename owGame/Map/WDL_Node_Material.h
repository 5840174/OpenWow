#pragma once

namespace
{
__declspec(align(16)) struct WDL_Node_MaterialMaterialProperties
{
	WDL_Node_MaterialMaterialProperties()
		: m_DiffuseColor(1, 1, 1, 1)
	{}
	glm::vec4   m_DiffuseColor;
	//-------------------------- ( 32 bytes )
};
}

class WDL_Node_Material 
	: public MaterialProxieT<WDL_Node_MaterialMaterialProperties>
{
public:
	WDL_Node_Material(IRenderDevice& RenderDevice);
	virtual ~WDL_Node_Material();

	const glm::vec4& GetDiffuseColor() const;
	void SetDiffuseColor(const glm::vec4& diffuse);	
};