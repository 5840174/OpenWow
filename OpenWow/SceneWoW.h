#pragma once

class CSceneWoW
	: public SceneBase
{
public:
	CSceneWoW(IBaseManager& BaseManager, IRenderWindow& RenderWindow);
	virtual ~CSceneWoW();

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
	std::shared_ptr<SkyManager> skyManager;
	std::shared_ptr<CMap> map;
	std::shared_ptr<IUIControlText> m_RendererStatisticText;
};


#if 0

class CSceneWoW
	: public SceneBase
{
public:
	CSceneWoW(IBaseManager& BaseManager);
	virtual ~CSceneWoW();

	// IGameState
	void Initialize() override;
	void Finalize() override;

	virtual void OnRayIntersected(const glm::vec3& Point);

	virtual void OnRender(RenderEventArgs& e) override;

	// Keyboard events
	virtual bool OnWindowKeyPressed(KeyEventArgs& e) override;
	virtual void OnWindowKeyReleased(KeyEventArgs& e) override;

private:
	void Load3D();
	void Load3D_M2s();
	void LoadUI();

	void TestCreateMap(uint32 mapID);
	void GoToCoord(const IUIControl* Node, const glm::vec2& Point);
	void TestDeleteMap();

private:
	//std::shared_ptr<CWMO_Base_Instance> wmoInstance;
	//std::shared_ptr<CM2_Base_Instance> m2Instance;
	std::shared_ptr<SkyManager> skyManager;
	std::shared_ptr<CMap> map;

	//std::shared_ptr<SceneNodeUI> rootForBtns;
	//std::shared_ptr<CUITextureNode> minimap;
};

#endif
