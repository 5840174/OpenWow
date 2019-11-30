#pragma once

class CGameState_World : public CGameState
{
	typedef CGameState base;
public:
	CGameState_World(const IApplication * _application);
	virtual ~CGameState_World();

	// IGameState
	bool Init();
	void Destroy();

    virtual void OnResize(ResizeEventArgs& e) override;

	virtual void OnPreRender(RenderEventArgs& e) override;
	virtual void OnRender(RenderEventArgs& e) override;
	virtual void OnPostRender(RenderEventArgs& e) override;
	virtual void OnRenderUI(RenderEventArgs& e) override;


private:
	void Load3D();
	void LoadUI();

	// Lights update
	void UpdateLights();

private:
	std::shared_ptr<CLight3D>               m_DirLight;
	std::shared_ptr<CMap>                   m_MapController;

	std::shared_ptr<CUITextNode>              m_CameraPosText;
	std::shared_ptr<CUITextNode>              m_CameraRotText;

    RenderTechnique                                 m_3D2Technique;
    Viewport m_Viewport2;
    std::shared_ptr<IRenderTarget>  m_RenderTarget;
    std::shared_ptr<ITexture>            m_Texture;

private:
	const uint16 c_WindowsWidth = 1280;
	const uint16 c_WindowsHeight = 1024;
	//const vec4 g_ClearColor = vec4(0.39f, 0.58f, 0.93f, 1.0f);
	const vec4 g_ClearColor = vec4(0.2f, 0.2f, 0.2f, 0.3f);
};