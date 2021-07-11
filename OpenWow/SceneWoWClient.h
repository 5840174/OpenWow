#pragma once

class CSceneWoWClient
	: public SceneBase
{
public:
	CSceneWoWClient(IBaseManager& BaseManager, IRenderWindow& RenderWindow);
	virtual ~CSceneWoWClient();

	// CSceneWoWClient
	void SetMainMenu();
	void RemoveMainMenu();

	// IScene
	void Initialize() override;
	void Finalize() override;

	void OnUpdate(UpdateEventArgs& e) override;

	bool OnMousePressed(const MouseButtonEventArgs & e, const Ray& RayToWorld) override;
	void OnMouseMoved(const MouseMotionEventArgs & e, const Ray& RayToWorld) override;

	// Keyboard events
	bool OnWindowKeyPressed(KeyEventArgs& e) override;
	void OnWindowKeyReleased(KeyEventArgs& e) override;

protected:
	void InitializeRenderer() override;

private:
	std::shared_ptr<CM2_Base_Instance> m_MainMenu;
	
	std::shared_ptr<ISceneNode> m_DefaultLightNode;
	std::shared_ptr<ISceneNode> m_DefaultCameraNode;

	std::unique_ptr<CWoWClient> m_WowClient;
	std::shared_ptr<IUIControlText> m_RendererStatisticText;
};