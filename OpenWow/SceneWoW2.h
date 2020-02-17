#pragma once

class CSceneWoW2
	: public SceneBase
{
public:
	CSceneWoW2(IBaseManager * BaseManager, const std::shared_ptr<ISceneNode3D>& Fake3DRootNode, const std::shared_ptr<ICameraComponent3D>& ExternalCamera);
	virtual ~CSceneWoW2();

	// IGameState
	void Initialize() override;
	void Finalize() override;

	virtual void OnRayIntersected(const glm::vec3& Point);

	virtual void OnPreRender(RenderEventArgs& e) override;
	virtual void OnRender(RenderEventArgs & e) override;

	// Keyboard events
	virtual bool OnWindowKeyPressed(KeyEventArgs& e) override;
	virtual void OnWindowKeyReleased(KeyEventArgs& e) override;

private:
	void Load3D();

private:
	CWMO_Base_Instance* wmoInstance;
	CM2_Base_Instance* m2Instance;
	SkyManager* skyManager;
	EnvironmentManager* environmentManager;
	CMap* map;
	WowTime wowGameTime;

	std::shared_ptr<ISceneNode3D> m_Fake3DRootNode;
	std::shared_ptr<ICameraComponent3D> m_FakeCamera;
};