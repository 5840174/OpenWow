#pragma once

class CGameState_Menu : public CGameState
{
	typedef CGameState base;
public:
    CGameState_Menu(const IApplication * _application);
	virtual ~CGameState_Menu();

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

private:
	std::shared_ptr<CFreeCameraController>  m_CameraController;

	std::shared_ptr<CUITextNode>            m_CameraPosText;
	std::shared_ptr<CUITextNode>            m_CameraRotText;


private:
	const uint16 c_WindowsWidth = 1280;
	const uint16 c_WindowsHeight = 1024;
	const vec4 g_ClearColor = vec4(0.4f, 0.2f, 0.2f, 0.3f);
};