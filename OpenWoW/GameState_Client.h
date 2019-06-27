#pragma once

class CGameState_Client : public CGameState
{
	typedef CGameState base;
public:
	                                                CGameState_Client(const IApplication * Application);
	virtual                                         ~CGameState_Client();

	void                                            S_CharEnum(CByteBuffer& _buff);

	// IGameState
	bool                                            Init();
	void                                            Destroy();

	virtual void                                    OnPreRender(RenderEventArgs& e);
	virtual void                                    OnRender(RenderEventArgs& e);
	virtual void                                    OnPostRender(RenderEventArgs& e);
	virtual void                                    OnRenderUI(RenderEventArgs& e);


private:
	void Load3D();
	void LoadUI();

private:
	std::shared_ptr<CMap>                           m_MapController;
	std::shared_ptr<CWoWClient>                     m_WoWClient;

	std::shared_ptr<CUITextNode>                    m_CameraPosText;
	std::shared_ptr<CUITextNode>                    m_CameraRotText;

private:
	const vec4 g_ClearColor = vec4(0.39f, 0.58f, 0.93f, 1.0f);
};