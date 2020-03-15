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
			: gColorLight(1, 1, 1)
			, gColorDark(1, 1, 1)
		{}
		float gShallowAlpha;
		float gDeepAlpha;
		glm::vec2 pad0;

		glm::vec3 gColorLight;
		float pad1;

		glm::vec3 gColorDark;
		float pad2;
		//-------------------------- ( 32 bytes )
	};
	MaterialProperties* m_pProperties;
};