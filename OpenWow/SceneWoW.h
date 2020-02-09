#pragma once

class CSceneWoW
	: public SceneBase
{
public:
	CSceneWoW(IBaseManager * BaseManager);
	virtual ~CSceneWoW();

	// IGameState
	void Initialize() override;
	void Finalize() override;

	virtual void OnRayIntersected(const glm::vec3& Point);

	virtual void OnPreRender(RenderEventArgs& e) override;

	// Keyboard events
	virtual bool OnWindowKeyPressed(KeyEventArgs& e) override;
	virtual void OnWindowKeyReleased(KeyEventArgs& e) override;

private:
	void Load3D();
	void LoadUI();

private:
	SkyManager* skyManager;
};