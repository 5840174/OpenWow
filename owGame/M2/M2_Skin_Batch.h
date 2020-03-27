#pragma once

#include "M2_Part_Material.h"
#include "M2_SkinSection.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

/**
  * M2 Batch что-то вроде прохода над геометрией. Имеет свои текстуры, цвета и тд.
  * Несколько батчей могут рисовать одну и ту же геометрию
*/
class CM2_Skin_Batch 
	: public MaterialProxie
{
public:
	CM2_Skin_Batch(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CM2& M2Model, const SM2_SkinBatch& SkinBatchProto);
	virtual ~CM2_Skin_Batch();

	const std::shared_ptr<const CM2_Part_Material>& GetM2Material() const {	return m_M2ModelMaterial; }
	void UpdateMaterialProps(const RenderEventArgs& RenderEventArgs, const CM2_Base_Instance* M2Instance);

public:
	int32												m_PriorityPlan;
	
	// Material props
	std::shared_ptr<const CM2_Part_Color>				m_Color;
	std::shared_ptr<const CM2_Part_Material>			m_M2ModelMaterial;
	std::vector<std::weak_ptr<const CM2_Part_Texture>>	m_Textures;
	int16												m_TextureUnit;
	std::shared_ptr<const CM2_Part_TextureWeight>		m_TextureWeight;
	std::shared_ptr<const CM2_Part_TextureTransform>	m_TextureTransform;

	int32												newShader;

private:
	__declspec(align(16)) struct ShaderM2BatchProperties
	{
		ShaderM2BatchProperties()
			: gBlendMode(0)
			, gShader(0)
			, gTextureAnimEnable(false)
			, gTextureWeight(1.0f)

			, gColor(glm::vec4(1.0))

			, gTextureAnimMatrix(glm::mat4(1.0f))

			, gInstanceColor(glm::vec4(1.0))
		{}

		uint32     gBlendMode;
		uint32     gShader;
		uint32     gTextureAnimEnable;
		float      gTextureWeight;
		//--------------------------------------------------------------( 16 bytes )

		glm::vec4  gColor;
		//--------------------------------------------------------------( 16 bytes )

		glm::mat4  gTextureAnimMatrix;
		//--------------------------------------------------------------( 16 * 4 bytes )

		glm::vec4  gInstanceColor;
		//--------------------------------------------------------------( 16 bytes )
	};

private:
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	const CM2& m_M2Model;
	const SM2_SkinBatch m_SkinBatchProto;
};
