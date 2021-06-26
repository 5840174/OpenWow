#pragma once

class CSceneWoWClient
	: public SceneBase
{
public:
	CSceneWoWClient(IBaseManager& BaseManager, IRenderWindow& RenderWindow);
	virtual ~CSceneWoWClient();

	// IGameState
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
	std::unique_ptr<CWoWClient> m_WowClient;
	std::shared_ptr<IUIControlText> m_RendererStatisticText;
};