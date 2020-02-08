#pragma once

#include "Map.h"

class CGameState_Map 
	: public IScene
{
public:
	CGameState_Map(IBaseManager * BaseManager, std::shared_ptr<IRenderWindow> RenderWindow, IWindowEvents* WindowEvents);
	virtual ~CGameState_Map();

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
	std::shared_ptr<CMap>                   m_MapController;
};