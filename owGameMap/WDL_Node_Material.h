#pragma once

class WDL_Node_Material : public MaterialWrapper
{
public:
	WDL_Node_Material();
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
		vec4   m_DiffuseColor;
		//-------------------------- ( 32 bytes )
	};
	MaterialProperties* m_pProperties;
};