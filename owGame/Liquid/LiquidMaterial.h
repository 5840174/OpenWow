#pragma once

class ZN_API LiquidMaterial
	: public MaterialProxie
{
public:
	LiquidMaterial(IRenderDevice& RenderDevice);
	virtual ~LiquidMaterial();

	// LiquidMaterial
	void SetShallowAlpha(float value);
	void SetDeepAlpha(float value);
	void SetColorLight(glm::vec3 value);
	void SetColorDark(glm::vec3 value);

protected:
	void UpdateConstantBuffer() const override;

private:
	__declspec(align(16)) struct MaterialProperties
	{
		MaterialProperties()
			: gColorLight(1.0f, 1.0f, 1.0f)
			, gColorDark(1.0f, 1.0f, 1.0f)
			, gShallowAlpha(1.0f)
			, gDeepAlpha(1.0f)
		{}
		glm::vec3 gColorLight;
		float gShallowAlpha;

		glm::vec3 gColorDark;
		float gDeepAlpha;
		//-------------------------- ( 32 bytes )
	};
	MaterialProperties* m_pProperties;
};