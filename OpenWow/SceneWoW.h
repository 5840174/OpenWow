#pragma once

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

	virtual void OnPreRender(RenderEventArgs& e) override;

	// Keyboard events
	virtual bool OnWindowKeyPressed(KeyEventArgs& e) override;
	virtual void OnWindowKeyReleased(KeyEventArgs& e) override;

private:
	void Load3D();
	void Load3D_M2s();
	void LoadUI();

	void TestCreateMap();
	void TestDeleteMap();

private:
	std::shared_ptr<CWMO_Base_Instance> wmoInstance;
	std::shared_ptr<CM2_Base_Instance> m2Instance;
	std::shared_ptr<SkyManager> skyManager;
	std::shared_ptr<CMap> map;
	

private: // Advanced
	struct QueryCreatures
	{
		std::string name;
		uint32 modelID;
	};
	std::map<uint64, QueryCreatures> m_EntriesName;

	std::map<uint64, std::shared_ptr<ISceneNode3D>> m_Objects;


};