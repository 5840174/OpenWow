#pragma once

#include "UIControls/UIControlMinimap.h"

class CSceneWoW
	: public SceneBase
{
public:
	CSceneWoW(IBaseManager& BaseManager, IRenderWindow& RenderWindow);
	virtual ~CSceneWoW();

	// CSceneWoWClient
	void SetMainMenu();
	void RemoveMainMenu();

	// IScene
	void Initialize() override;
	void Finalize() override;

	virtual void OnUpdate(UpdateEventArgs& e) override;

	virtual bool OnMousePressed(const MouseButtonEventArgs & e, const Ray& RayToWorld) override;
	virtual void OnMouseMoved(const MouseMotionEventArgs & e, const Ray& RayToWorld) override;

	// Keyboard events
	virtual bool OnWindowKeyPressed(KeyEventArgs& e) override;
	virtual void OnWindowKeyReleased(KeyEventArgs& e) override;


protected:
	virtual void InitializeRenderer() override;


private:
	void M2Test();
	void GoToCoord(const IUIControl* Node, const glm::vec2& Point);


private:
	std::unique_ptr<CWorldClient> m_WorldClient;

	std::shared_ptr<IUIControlText> m_RendererStatisticText;
	std::shared_ptr<IUIControlText> m_AreaName;


	std::shared_ptr<CUIControlMinimap> minimap;

	std::shared_ptr<ISceneNode> m_DefaultLightNode;
	std::shared_ptr<ISceneNode> m_DefaultCameraNode;

	std::shared_ptr<CM2_Base_Instance> m_WoWCameraNode;

	std::shared_ptr<ISceneNode> m_DebugBall;
};
