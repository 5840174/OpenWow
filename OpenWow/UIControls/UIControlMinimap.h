#pragma once


class CUIControlMinimap
	: public CUIControlCommon
{
public:
	ZN_OBJECTCLASS(cUIControlMinimap);

	CUIControlMinimap(IScene& Scene);
	virtual ~CUIControlMinimap();

	// CUIControl
	void Initialize() override;

	// CUIControlButton
	void SetMinimapTexture(std::shared_ptr<ITexture> Texture);

	// Syntetic events
	void OnMouseEntered() override;
	void OnMouseLeaved() override;

private:
	std::shared_ptr<IUIControlCommonModel> m_MinimapGeometry;
};