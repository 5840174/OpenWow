#pragma once

class WDL_Node_Material 
	: public MaterialProxie
{
public:
	WDL_Node_Material(IRenderDevice& RenderDevice);
	virtual ~WDL_Node_Material();

	cvec4 GetDiffuseColor() const;
	void SetDiffuseColor(cvec4 diffuse);

protected:
	void UpdateConstantBuffer() const;

private:
	__declspec(align(16)) struct MaterialProperties
	{
		MaterialProperties()
			: m_DiffuseColor(1, 1, 1, 1)
		{}
		glm::vec4   m_DiffuseColor;
		//-------------------------- ( 32 bytes )
	};
	MaterialProperties* m_pProperties;
};