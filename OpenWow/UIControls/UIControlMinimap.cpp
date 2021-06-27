#include "stdafx.h"

// General
#include "UIControlMinimap.h"


CUIControlMinimap::CUIControlMinimap(IScene& Scene)
	: CUIControlCommon(Scene)
{
}

CUIControlMinimap::~CUIControlMinimap()
{
}



//
// CUIControlCommon
//
void CUIControlMinimap::Initialize()
{
	__super::Initialize();

	std::shared_ptr<CUICommonMaterial> backgroundMaterial = MakeShared(CUICommonMaterial, GetBaseManager().GetApplication().GetRenderDevice());
	backgroundMaterial->SetColor(ColorRGBA(0.0f, 0.0f, 0.0f, 0.5f));

	auto subGeom = MakeShared(CUICommonModel, GetRenderDevice());
	subGeom->SetTranslate(glm::vec2(0.0f, 0.0f));
	subGeom->SetSize(glm::vec2(256));
	subGeom->SetMaterial(backgroundMaterial);
	subGeom->SetGeom(GetBaseManager().GetApplication().GetRenderDevice().GetPrimitivesFactory().CreateUIQuad(glm::vec2(1.0f)));
	AddSubgeometry(subGeom);

	std::shared_ptr<CUICommonMaterial> minimapMaterial = MakeShared(CUICommonMaterial, GetBaseManager().GetApplication().GetRenderDevice());
	minimapMaterial->SetColor(ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	m_MinimapGeometry = MakeShared(CUICommonModel, GetRenderDevice());
	m_MinimapGeometry->SetTranslate(glm::vec2(0.0f));
	m_MinimapGeometry->SetSize(glm::vec2(256));
	m_MinimapGeometry->SetMaterial(minimapMaterial);
	m_MinimapGeometry->SetGeom(GetBaseManager().GetApplication().GetRenderDevice().GetPrimitivesFactory().CreateUIQuad(glm::vec2(1.0f)));
	AddSubgeometry(m_MinimapGeometry);

	SetSize(glm::vec2(256));
}

void CUIControlMinimap::SetMinimapTexture(std::shared_ptr<ITexture> Texture)
{
	m_MinimapGeometry->GetMaterial()->SetTexture(Texture);
}

void CUIControlMinimap::OnMouseEntered()
{
	for (auto& subGeometry : GetSubgeometries())
	{
		if (subGeometry == m_MinimapGeometry)
			continue;

		std::dynamic_pointer_cast<CUICommonMaterial>(subGeometry->GetMaterial())->SetColor(ColorRGBA(0.0f, 0.0f, 0.0f, 0.3f));
	}
}

void CUIControlMinimap::OnMouseLeaved()
{
	for (auto& subGeometry : GetSubgeometries())
	{
		if (subGeometry == m_MinimapGeometry)
			continue;

		std::dynamic_pointer_cast<CUICommonMaterial>(subGeometry->GetMaterial())->SetColor(ColorRGBA(0.0f, 0.0f, 0.0f, 0.5f));
	}
}
