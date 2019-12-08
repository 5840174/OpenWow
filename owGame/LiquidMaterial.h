#pragma once

class OW_ENGINE_API LiquidMaterial
	: public MaterialProxie
{
public:
	LiquidMaterial(IBaseManager* BaseManager);
	virtual ~LiquidMaterial();

	void SetShallowAlpha(float value);
	void SetDeepAlpha(float value);
	void SetColorLight(vec3 value);
	void SetColorDark(vec3 value);

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
		vec2 pad0;

		vec3 gColorLight;
		float pad1;

		vec3 gColorDark;
		float pad2;
		//-------------------------- ( 32 bytes )
	};
	MaterialProperties* m_pProperties;
};